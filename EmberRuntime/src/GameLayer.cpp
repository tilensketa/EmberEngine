#include "GameLayer.h"

#define RENDER_RUNTIME_ID 5
#define RUNTIME_SHADERS "../../EmberRuntime/res/shaders/"

namespace Ember {
struct ScreenVertex {
  glm::vec2 position;
  glm::vec2 texCoord;
};

GameLayer::GameLayer(ProjectManager &projectManager,
                     std::shared_ptr<Window> window)
    : mProjectManager(projectManager), mWindow(window) {
  mRenderTarget = std::make_unique<RenderTarget>(1000, 1000);
  mScreenShader =
      std::make_unique<Shader>(RUNTIME_SHADERS, "quad.vert", "quad.frag");

  std::vector<ScreenVertex> vertices = {
      ScreenVertex{glm::vec2(-1.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
      ScreenVertex{glm::vec2(1.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
      ScreenVertex{glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
      ScreenVertex{glm::vec2(-1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
  };
  std::vector<GLuint> indices = {0, 1, 2, 2, 3, 0};

  mScreenVao = std::make_unique<VAO>();
  mScreenVao->Bind();
  std::shared_ptr<VBO<ScreenVertex>> vbo =
      std::make_shared<VBO<ScreenVertex>>(vertices);
  std::shared_ptr<EBO> Ebo = std::make_shared<EBO>(indices);
  mScreenVao->LinkAttrib(*vbo, 0, 2, GL_FLOAT, sizeof(ScreenVertex), (void *)0);
  mScreenVao->LinkAttrib(*vbo, 1, 2, GL_FLOAT, sizeof(ScreenVertex),
                         (void *)(2 * sizeof(float)));
  mScreenVao->Unbind();
}

GameLayer::~GameLayer() {}

void GameLayer::OnAttach() {}
void GameLayer::OnDetach() {}
void GameLayer::OnUpdate() {
  auto scene = mProjectManager.GetSceneManager().GetCurrentScene();
  if (!scene) {
    return;
  }

  auto cameraView = scene->View<Component::Camera, Component::Transform>();
  if (cameraView.empty()) {
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

  Event::RenderReqeust event(RENDER_RUNTIME_ID, renderContext);
  Application::Get().DispatchEvent(event);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, mWindow->GetWidth(), mWindow->GetHeight());
  glDisable(GL_DEPTH_TEST);

  mScreenShader->Activate();
  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, mRenderTarget->GetTextureId());
  mScreenShader->SetInt("uTexture", 5);
  mScreenShader->SetFloat("uCameraAspect", activeCamera.aspect);
  float windowAspect = float(mWindow->GetWidth()) / float(mWindow->GetHeight());
  mScreenShader->SetFloat("uScreenAspect", windowAspect);

  mScreenVao->Bind();
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
  mScreenVao->Unbind();
  glEnable(GL_DEPTH_TEST);
}

void GameLayer::OnEvent(Event::IEvent &event) {
  if (event.GetType() == Event::EventType::RENDER_COMPLETE) {
    if (auto *renderCompleteEvent =
            dynamic_cast<Event::RenderComplete *>(&event)) {
      if (renderCompleteEvent->id == RENDER_RUNTIME_ID) {
        mRenderTarget = std::make_unique<RenderTarget>(
            *renderCompleteEvent->renderContext.renderTarget);
      }
    }
    event.handled = true;
  }
}
} // namespace Ember
