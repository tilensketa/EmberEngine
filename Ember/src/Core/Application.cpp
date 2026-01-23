#include "Application.h"

#include "Layers/RenderLayer.h"
#include "Logger.h"

#include "Project/Assets/EngineAssetLoader.h"

namespace Ember {
Application *Application::sInstance = nullptr;

Application::Application(const ApplicationSpecification &specification) {
  Logger::Init();
  mWindow = std::make_shared<Window>(specification.windowSpec);
  mWindow->SetEventCallback(
      [this](Event::IEvent &e) { mLayerStack->DispatchEvent(e); });

  mAssetManager = std::make_unique<AssetManager>("");
  EngineAssetLoader::LoadBuiltInAssets(*mAssetManager);
  mAssetRegistry.RegisterDomain(AssetDomain::Engine, mAssetManager.get());
  mProjectManager = std::make_unique<ProjectManager>(mAssetRegistry);
  mLayerStack = std::make_shared<LayerStack>();
  mRenderLayer =
      std::make_shared<RenderLayer>(*mProjectManager, mAssetRegistry);
  mLayerStack->PushLayer(mRenderLayer);
  sInstance = this;

  LOG_INFO("HELLO FROM EMBERENGINE");
}
Application::~Application() {}
void Application::Run() {
  while (!mWindow->ShouldClose()) {
    mWindow->UpdateBegin();
    mLayerStack->OnUpdate();
    mWindow->UpdateEnd();
  }
}
void Application::Stop() {}

Application &Application::Get() { return *sInstance; }
void Application::DispatchEvent(Event::IEvent &event) {
  mLayerStack->DispatchEvent(event);
}
} // namespace Ember
