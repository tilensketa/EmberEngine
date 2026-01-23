#include "SceneViewPanel.h"
#include <cstdint>

#include <glm/gtc/type_ptr.hpp>

#include <ImGuizmo.h>

#define RENDER_SCENE_ID 1

namespace Ember {
void SceneViewPanel::Update() {
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
  ImGui::Begin("Scene");
  ImGui::PopStyleVar(2);

  auto scene = mCtx.projectManager.GetSceneManager().GetCurrentScene();
  if(!scene){
    ImGui::End();
    return;
  }

  // Request render scene
  auto camera = mCtx.editorState.temporary.camCamera;
  mRenderTarget->Resize(camera.resolution.x, camera.resolution.y);

  RenderContext renderContext;
  renderContext.renderTarget = mRenderTarget.get();
  renderContext.scene = scene;
  renderContext.camera = camera;
  renderContext.cameraTransform = mCtx.editorState.temporary.camTransform;

  Event::RenderReqeust event(RENDER_SCENE_ID, renderContext);
  Application::Get().DispatchEvent(event);

  handleEditor();

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

  showGizmo();
  showViewportTools();

  ImGui::End();
}
void SceneViewPanel::OnEvent(Ember::Event::IEvent &event) {
  if (event.GetType() == Event::EventType::RENDER_COMPLETE) {
    if (auto *renderCompleteEvent =
            dynamic_cast<Event::RenderComplete *>(&event)) {
      if (renderCompleteEvent->id == RENDER_SCENE_ID) {
        mRenderTarget = std::make_unique<RenderTarget>(
            *renderCompleteEvent->renderContext.renderTarget);
      }
    }
    event.handled = true;
  }
}

void SceneViewPanel::showGizmo() {
  ImGuizmo::BeginFrame();

  ImVec2 imagePos = ImGui::GetItemRectMin();
  ImVec2 imageSize = ImGui::GetItemRectSize();

  ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());

  ImGuizmo::SetRect(imagePos.x, imagePos.y, imageSize.x, imageSize.y);

  auto scene = mCtx.projectManager.GetSceneManager().GetCurrentScene();
  if (!scene) return;
  auto &editorState = mCtx.editorState;

  auto &camera = editorState.temporary.camCamera;
  auto &transform = editorState.temporary.camTransform;

  glm::mat4 axisMatrix = glm::mat4(0.0f);
  axisMatrix[0][2] = 1.0f;
  axisMatrix[1][0] = 1.0f;
  axisMatrix[2][1] = 1.0f;
  axisMatrix[3][3] = 1.0f;
  glm::mat4 camView = axisMatrix * glm::inverse(transform.GetMatrix());
  glm::mat4 camProj = camera.GetProjection();

  const auto &selected = editorState.temporary.selectedEntityHandle;
  if (selected && scene->HasComponent<Component::Transform>(*selected)) {
    auto &transform = scene->Get<Component::Transform>(*selected);
    glm::mat4 model = transform.GetMatrix();
    ImGuizmo::Manipulate(glm::value_ptr(camView), glm::value_ptr(camProj),
                         editorState.temporary.currentOperation,
                         editorState.temporary.currentMode,
                         glm::value_ptr(model));

    if (ImGuizmo::IsUsing()) {
      transform.SetMatrix(model);
    }
  }
}

void SceneViewPanel::showViewportTools() {
  ImVec2 padding = ImVec2(10, 30);
  ImVec2 windowPos = ImGui::GetWindowPos();
  ImVec2 panelPos = ImVec2(windowPos.x + padding.x, windowPos.y + padding.y);

  ImGui::SetNextWindowPos(panelPos);
  ImGui::SetNextWindowBgAlpha(0.85f);

  ImGuiWindowFlags flags =
      ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize |
      ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings |
      ImGuiWindowFlags_NoFocusOnAppearing;

  ImGui::Begin("ViewportTools", nullptr, flags);

  auto &editorState = mCtx.editorState;

  ImGuizmo::OPERATION &op = editorState.temporary.currentOperation;
  if (ImGui::Selectable("T", op == ImGuizmo::TRANSLATE))
    op = ImGuizmo::TRANSLATE;
  if (ImGui::Selectable("R", op == ImGuizmo::ROTATE)) op = ImGuizmo::ROTATE;
  if (ImGui::Selectable("S", op == ImGuizmo::SCALE)) op = ImGuizmo::SCALE;

  ImGuizmo::MODE &mode = editorState.temporary.currentMode;
  if (ImGui::Selectable("Local", mode == ImGuizmo::LOCAL))
    mode = ImGuizmo::LOCAL;
  if (ImGui::Selectable("World", mode == ImGuizmo::WORLD))
    mode = ImGuizmo::WORLD;
  ImGui::End();
}
void SceneViewPanel::handleEditor() {
  if (!ImGui::IsWindowHovered()) return;

  auto &editorState = mCtx.editorState;
  float &speed = editorState.temporary.camSpeed;
  auto &transform = editorState.temporary.camTransform;

  constexpr glm::vec3 WORLD_UP = {0, 0, 1};

  glm::vec3 forward = transform.rotation * glm::vec3(-1, 0, 0);
  glm::vec3 right = transform.rotation * glm::vec3(0, 1, 0);
  glm::vec3 up = glm::vec3(0, 0, 1);

  // TODO add delta time
  if (Input::IsKeyDown(GLFW_KEY_W)) transform.position += forward * speed;
  if (Input::IsKeyDown(GLFW_KEY_S)) transform.position -= forward * speed;
  if (Input::IsKeyDown(GLFW_KEY_D)) transform.position += right * speed;
  if (Input::IsKeyDown(GLFW_KEY_A)) transform.position -= right * speed;
  if (Input::IsKeyDown(GLFW_KEY_SPACE)) transform.position += up * speed;
  if (Input::IsKeyDown(GLFW_KEY_LEFT_CONTROL)) transform.position -= up * speed;

  // TODO rework
  // ---------- Rotation ----------
  static bool rotating = false;
  static double lastX = 0.0, lastY = 0.0;

  if (Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)) {
    double x = Input::GetMouseX();
    double y = Input::GetMouseY();

    if (!rotating) {
      lastX = x;
      lastY = y;
      rotating = true;
      return;
    }

    float dx = float(x - lastX);
    float dy = float(y - lastY);

    lastX = x;
    lastY = y;

    const float sensitivity = 0.002f;

    // yaw: rotate around world Z
    glm::quat yaw = glm::angleAxis(-dx * sensitivity, WORLD_UP);

    // pitch: rotate around camera right (local Y)
    glm::vec3 camRight = transform.rotation * glm::vec3(0, 1, 0);

    glm::quat pitch = glm::angleAxis(-dy * sensitivity, camRight);

    // Apply yaw (world) then pitch (local)
    transform.rotation = glm::normalize(yaw * pitch * transform.rotation);
  } else {
    rotating = false;
  }
}
} // namespace Ember
