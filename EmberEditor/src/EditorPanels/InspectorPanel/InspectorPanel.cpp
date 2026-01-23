#include "InspectorPanel.h"

namespace Ember {

void InspectorPanel::Update() {
  ImGui::Begin("Inspector");
  auto scene = mCtx.projectManager.GetSceneManager().GetCurrentScene();
  if (!scene || !mCtx.editorState.temporary.selectedEntityHandle) {
    ImGui::End();
    return;
  }
  const EntityHandle &handle = *mCtx.editorState.temporary.selectedEntityHandle;
  // auto &id = scene->Get<Component::Id>(handle);
  // auto &tag = scene->Get<Component::Tag>(handle);

  drawComponent<Component::AllComponents>(scene.get(), handle);

  if (ImGui::Button("Add Component",
                    ImVec2(ImGui::GetContentRegionAvail().x, 0)))
    mShowAddComponent = !mShowAddComponent;

  if (mShowAddComponent) {
    ImGui::BeginChild("AddComponentChild",
                      ImVec2(ImGui::GetContentRegionAvail().x, 260),
                      ImGuiChildFlags_Borders);
    ImGui::Text("\xee\xa9\xad");
    ImGui::SameLine();
    static char buffer[32];
    ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::InputText("###Search", buffer, sizeof(buffer));
    ImGui::SeparatorText("COMPONENTS");
    showAddComponentDropdown<Component::AllComponents>(scene.get(), handle,
                                                       buffer);
    ImGui::EndChild();
  }

  ImGui::End();
}
void InspectorPanel::OnEvent(Ember::Event::IEvent &event) {}
} // namespace Ember
