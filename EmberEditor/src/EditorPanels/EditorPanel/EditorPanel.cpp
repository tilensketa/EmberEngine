#include "EditorPanel.h"

#include "EditorPanels/InspectorPanel/Inspector.h"

namespace Ember {

void EditorPanel::Update() {
  ImGui::Begin("Editor");
  auto &temp = mCtx.editorState.temporary;
  ImGui::Text("Selected Folder: %s", temp.selectedFolder.c_str());
  Inspector::DrawComponent<Component::Transform>(temp.camTransform, mCtx);
  Inspector::DrawComponent<Component::Camera>(temp.camCamera, mCtx);
  ImGui::DragFloat("CamSpeed", &temp.camSpeed, 0.1f, 0.1f, 10.0f);
  ImGui::End();
}

void EditorPanel::OnEvent(Ember::Event::IEvent &event) {}

} // namespace Ember
