#include "AssetRegistryPanel.h"

namespace Ember {

void AssetRegistryPanel::Update() {
  ImGui::Begin("AssetRegistry");

  auto showHandles = [&](const std::vector<AssetHandle> &handles,
                         const std::string &filter) {
    for (const AssetHandle &handle : handles) {
      std::string handleString = handle.path.string() +
                                 std::to_string(handle.guid.GetID()) +
                                 Utils::ToString<AssetType>(handle.type);
      auto it = handleString.find(filter);
      if (it != handleString.npos || filter.empty()) {
        ImGui::Text("GUID: %lu", handle.guid.GetID());
        ImGui::Text("Path: %s", handle.path.c_str());
        ImGui::Text("Type: %s", Utils::ToString<AssetType>(handle.type));
        ImGui::Separator();
      }
    }
  };

  static char buffer[128];
  ImGui::InputText("##SearchAsset", buffer, sizeof(buffer));
  std::string filter(buffer);

  ImGui::Separator();
  auto engineAssetManager =
      mCtx.assetRegistry.GetAssetManagerByDomain(AssetDomain::Engine);
  if (engineAssetManager) {
    if (ImGui::CollapsingHeader("ENGINE DOMAIN ASSETS",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      showHandles(engineAssetManager->GetAllAssetHandles(), filter);
    }
  }
  ImGui::Separator();
  auto projectAssetManager =
      mCtx.assetRegistry.GetAssetManagerByDomain(AssetDomain::Project);
  if (projectAssetManager) {
    if (ImGui::CollapsingHeader("PROJECT DOMAIN ASSETS",
                                ImGuiTreeNodeFlags_DefaultOpen)) {
      showHandles(projectAssetManager->GetAllAssetHandles(), filter);
    }
  }
  ImGui::End();
}

void AssetRegistryPanel::OnEvent(Ember::Event::IEvent &event) {};
} // namespace Ember
