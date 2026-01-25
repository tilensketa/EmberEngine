#include "TopBarPanel.h"

namespace Ember {

void TopBarPanel::Update() {
  ImGui::Begin("TopBar");

  ImGui::Text("Physics: ");
  ImGui::SameLine();
  if (ImGui::Button("Start")) {
    auto &sceneManager = mCtx.projectManager.GetSceneManager();
    auto newGuid = sceneManager.DuplicateCurrentScene();

    Event::PhysicsStart event(sceneManager.GetCurrentScene());
    Application::Get().DispatchEvent(event);
  }
  ImGui::SameLine();
  if (ImGui::Button("Pause")) {
    Event::PhysicsPause event;
    Application::Get().DispatchEvent(event);
  }
  ImGui::SameLine();
  if (ImGui::Button("UnPause")) {
    Event::PhysicsUnPause event;
    Application::Get().DispatchEvent(event);
  }
  ImGui::SameLine();
  if (ImGui::Button("Stop")) {
    auto &sceneManager = mCtx.projectManager.GetSceneManager();
    auto guid = sceneManager.GetCurrentScene()->GetGuid();
    sceneManager.RemoveScene(guid);
    sceneManager.ChangeScene(GUID(guid.GetID() - 1));

    Event::PhysicsStop event;
    Application::Get().DispatchEvent(event);
  }

  ImGui::End();
}

void TopBarPanel::OnEvent(Ember::Event::IEvent &event) {};

} // namespace Ember
