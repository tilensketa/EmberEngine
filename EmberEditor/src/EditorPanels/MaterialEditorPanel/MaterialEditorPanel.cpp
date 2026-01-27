#include "MaterialEditorPanel.h"

#include "EditorPanels/EditorHelpers.h"
#include <Ember.h>

namespace Ember {

void MaterialEditorPanel::Update() {
  ImGui::Begin("MaterialEditor");
  std::optional<GUID> matGuid = mCtx.editorState.temporary.selectedMaterial;
  if (!matGuid) {
    ImGui::End();
    return;
  }
  auto matHandleOpt = mCtx.assetRegistry.GetAssetHandleByGuid(*matGuid);
  if (!matHandleOpt) {
    LOG_ERROR(
        "MaterialEditorPanel: MatGuid: {}, but no handle in AssetRegistry",
        *matGuid);
    ImGui::End();
    return;
  }
  auto material = mCtx.assetRegistry.GetAsset<Asset::Material>(*matGuid);
  if (!material) {
    LOG_ERROR("MaterialEditorPanel: MatGuid: {}, handle: {}, but no material "
              "asset in AssetRegistry",
              *matGuid, *matHandleOpt);
    ImGui::End();
    return;
  }

  const AssetHandle &matHandle = *matHandleOpt;
  Asset::Material &mat = *material;

  ImGui::Text("Material: %s", matHandle.path.c_str());
  bool changed = false;
  ImGui::BeginDisabled(
      matGuid ==
      EngineAssetLoader::GetMaterial(EngineMaterial::DefaultMaterial).guid);

  if (ImGui::CollapsingHeader("Albedo", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= ImGui::ColorEdit3("Color", &mat.factors.albedoColor.x);
    changed |=
        ImGui::DragFloat("Emission", &mat.factors.emission, 0.01f, 0.0f, 1.0f);
    changed |= handleTexture("###AlbedoMap", mat.albedoMap);
  }
  if (ImGui::CollapsingHeader("Normal", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= handleTexture("###NormalMap", mat.normalMap);
  }
  if (ImGui::CollapsingHeader("Metalness", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= ImGui::DragFloat("###Metalness", &mat.factors.metalness, 0.01f,
                                0.0f, 1.0f);
    changed |= handleTexture("###MetalnessMap", mat.metalnessMap);
  }
  if (ImGui::CollapsingHeader("Roughness", ImGuiTreeNodeFlags_DefaultOpen)) {
    changed |= ImGui::DragFloat("###Roughness", &mat.factors.roughness, 0.01f,
                                0.0f, 1.0f);
    changed |= handleTexture("###RoughnessMap", mat.roughnessMap);
  }
  if (changed) {
    auto matFile =
        mCtx.projectManager.GetProjectDirectories().assets / matHandle.path;
    MaterialLoader::ToFile(mat, matFile);
  }
  ImGui::EndDisabled();
  ImGui::End();
}

void MaterialEditorPanel::OnEvent(Ember::Event::IEvent &event) {};

bool MaterialEditorPanel::handleTexture(const std::string &name, GUID &guid) {
  ImGui::PushID(name.c_str());
  // ImGui::Text("%s", name.c_str());

  AssetHandle nullTexHandle =
      EditorAssetLoader::GetIcon(EditorIcon::NullTexture);
  auto texture = mCtx.assetManager.GetAsset<Asset::Texture>(nullTexHandle.guid);
  if (guid != GUID::NONE()) {
    texture = mCtx.assetRegistry.GetAsset<Asset::Texture>(guid);
  }
  if (!texture) {
    ImGui::PopID();
    return false;
  }

  ImGui::PushStyleColor(ImGuiCol_Button, {0, 0, 0, 0});
  if (ImGui::ImageButton("##img", (ImTextureID)(intptr_t)texture->gpu.id,
                         ImVec2(150.0f, 150.0f))) {
    ImGui::OpenPopup("TextureSelector");
  }
  ImGui::PopStyleColor();

  bool changed = false;
  auto selectedHandle = EditorHelpers::AssetSelector(
      mCtx.assetRegistry, AssetType::TEXTURE, "TextureSelector", true);
  if (selectedHandle) {
    guid = selectedHandle->guid;
    changed = true;
  }
  ImGui::PopID();
  return changed;
}

} // namespace Ember
