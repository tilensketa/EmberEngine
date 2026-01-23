#include "GameViewPanel.h"
#include <cstdint>

#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

#define RENDER_GAME_ID 2

namespace Ember {
void GameViewPanel::Update() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  ImGui::Begin("Game");
  ImGui::PopStyleVar(2);

  // Request render scene
  auto scene = mCtx.projectManager.GetSceneManager().GetCurrentScene();
  if (!scene) {
    ImGui::End();
    return;
  }

  auto cameraView = scene->View<Component::Camera, Component::Transform>();
  if (cameraView.empty()) {
    ImGui::End();
    return;
  }

  Component::Camera activeCamera;
  Component::Transform activeCameraTransform;
  for (const auto &cameraEntity : cameraView) {
    auto camera = scene->Get<Component::Camera>(cameraEntity);
    auto cameraTransform = scene->Get<Component::Transform>(cameraEntity);
    camera.aspect = float(camera.resolution.x) / float(camera.resolution.y);

    if (camera.active) {
      mRenderTarget->Resize(camera.resolution.x, camera.resolution.y);
      activeCamera = camera;
      activeCameraTransform = cameraTransform;
      break;
    }
  }

  RenderContext renderContext;
  renderContext.renderTarget = mRenderTarget.get();
  renderContext.scene = scene;
  renderContext.camera = activeCamera;
  renderContext.cameraTransform = activeCameraTransform;

  Event::RenderReqeust event(RENDER_GAME_ID, renderContext);
  Application::Get().DispatchEvent(event);

  if (!mRenderTarget) {
    ImGui::End();
    return;
  }

  ImVec2 windowSize = ImGui::GetContentRegionAvail();
  glm::vec2 imageSize =
      glm::vec2(mRenderTarget->GetWidth(), mRenderTarget->GetHeight());
  float aspectRatio = imageSize.x / imageSize.y;

  float newWidth = windowSize.x;
  float newHeight = windowSize.x / aspectRatio;
  if (newHeight > windowSize.y) {
    newHeight = windowSize.y;
    newWidth = windowSize.y * aspectRatio;
  }

  ImVec2 size = ImVec2(newWidth, newHeight);
  ImVec2 offset =
      ImVec2((windowSize.x - size.x) * 0.5f, (windowSize.y - size.y) * 0.5f);
  ImGui::SetCursorPos(offset);

  mRenderTarget->Bind();
  ImGui::Image((ImTextureID)(intptr_t)mRenderTarget->GetTextureId(), size);
  mRenderTarget->Unbind();

  ImGui::End();
}
void GameViewPanel::OnEvent(Ember::Event::IEvent &event) {
  if (event.GetType() == Event::EventType::RENDER_COMPLETE) {
    if (auto *renderCompleteEvent =
            dynamic_cast<Event::RenderComplete *>(&event)) {
      if (renderCompleteEvent->id == RENDER_GAME_ID) {
        mRenderTarget = std::make_unique<RenderTarget>(
            *renderCompleteEvent->renderContext.renderTarget);
      }
    }
    event.handled = true;
  }
}
} // namespace Ember
