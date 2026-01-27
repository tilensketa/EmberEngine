#include "HierarchyPanel.h"

namespace Ember {

void HierarchyPanel::Update() {
  ImGui::Begin("Hierarchy");
  auto scene = mCtx.projectManager.GetSceneManager().GetCurrentScene();
  if (!scene) {
    ImGui::End();
    return;
  }
  ImGui::Text("Scene: %s", scene->GetName().c_str());
  auto &selectedEntityHandle = mCtx.editorState.temporary.selectedEntityHandle;
  auto view = scene->View<Component::Id, Component::Tag>();

  ImVec2 childSize = ImVec2(0, ImGui::GetContentRegionAvail().y);
  ImGui::BeginChild("Entities", childSize, ImGuiChildFlags_Borders);

  handleAddMenu(scene);

  std::optional<EntityHandle> entityToDelete = std::nullopt;
  for (auto entity : view) {
    auto &id = scene->Get<Component::Id>(entity);
    auto &tag = scene->Get<Component::Tag>(entity);
    ImGui::PushID(&id.guid);

    bool showEntity = true;
    bool isActive = selectedEntityHandle && *selectedEntityHandle == entity;
    if (isActive) {
      ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.3f, 0.5f, 1.0f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                            ImVec4(0.4f, 0.6f, 1.0f, 1.0f));
      ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                            ImVec4(0.2f, 0.4f, 0.9f, 1.0f));
    }
    ImGui::CollapsingHeader(tag.value.c_str(), &showEntity,
                            ImGuiTreeNodeFlags_Bullet);
    if (!showEntity) entityToDelete = entity;
    if (ImGui::IsItemClicked()) selectedEntityHandle = entity;

    if (isActive) ImGui::PopStyleColor(3);

    ImGui::PopID();
  }

  if (entityToDelete) {
    scene->RemoveEntity(*entityToDelete);
    selectedEntityHandle = std::nullopt;
  }

  ImGui::EndChild();
  ImGui::End();
}

void HierarchyPanel::OnEvent(Ember::Event::IEvent &event) {};

void HierarchyPanel::handleAddMenu(std::shared_ptr<Scene> scene) {
  if (ImGui::BeginPopupContextWindow("HierarchyAddPopup",
                                     ImGuiPopupFlags_MouseButtonRight)) {
    auto &selectedEntityHandle =
        mCtx.editorState.temporary.selectedEntityHandle;

    ImGui::Text("ADD");
    ImGui::Separator();

    if (ImGui::BeginMenu("Model")) {
      for (auto desc : sEnginePrimitives) {
        if (ImGui::MenuItem(desc.name)) {
          EntityHandle e = scene->CreateEntity(desc.name);
          scene->AddComponent<Component::Model>(
              e, {EngineAssetLoader::GetPrimitive(desc.primitive),
                  {GUID::NONE()}});
          selectedEntityHandle = e;
        }
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Empty")) {
      if (ImGui::MenuItem("Empty")) {
        EntityHandle e = scene->CreateEntity("Empty");
        selectedEntityHandle = e;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Camera")) {
      if (ImGui::MenuItem("Camera")) {
        EntityHandle e = scene->CreateEntity("Camera");
        scene->AddComponent<Component::Camera>(e);
        selectedEntityHandle = e;
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Light")) {
      if (ImGui::MenuItem("Point")) {
        EntityHandle e = scene->CreateEntity("PointLight");
        scene->AddComponent<Component::Light>(e, Component::Light());
        selectedEntityHandle = e;
      }
      ImGui::EndMenu();
    }

    ImGui::EndPopup();
  }
}
} // namespace Ember
