#include "Application.h"

#include "Events/Events.h"
#include "Input.h"

#include "Layers/RenderLayer.h"
#include "Logger.h"

#include "Project/Assets/EngineAssetLoader.h"

namespace Ember {
Application *Application::sInstance = nullptr;

Application::Application(const ApplicationSpecification &specification) {
  Logger::Init();
  mWindow = std::make_shared<Window>(specification.windowSpec);
  mWindow->SetEventCallback([this](Event::IEvent &e) { DispatchEvent(e); });

  mAssetManager = std::make_unique<AssetManager>("");
  EngineAssetLoader::LoadBuiltInAssets(*mAssetManager);
  mAssetRegistry.RegisterDomain(AssetDomain::Engine, mAssetManager.get());
  mProjectManager = std::make_unique<ProjectManager>(mAssetRegistry);

  mLayerStack = std::make_shared<LayerStack>();
  mRenderLayer =
      std::make_shared<RenderLayer>(*mProjectManager, mAssetRegistry);
  mLayerStack->PushLayer(mRenderLayer);
  mPhysicsLayer = std::make_shared<PhysicsLayer>();
  mLayerStack->PushLayer(mPhysicsLayer);
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
  // LOG_DEBUG("Event: {}", event.ToString());
  EventDispatcher dispatcher(event);

  // Window events
  dispatcher.Dispatch<Event::WindowResize>(
      Event::EventType::WINDOW_RESIZE, [this](Event::WindowResize &e) {
        mWindow->Resize(e.width, e.height);
        glViewport(0, 0, e.width, e.height);
        return true;
      });
  // Key events
  dispatcher.Dispatch<Event::KeyPress>(Event::EventType::KEY_PRESS,
                                       [this](Event::KeyPress &e) {
                                         Input::OnKeyEvent(e.keycode, true);
                                         return false;
                                       });
  dispatcher.Dispatch<Event::KeyRelease>(Event::EventType::KEY_RELEASE,
                                         [this](Event::KeyRelease &e) {
                                           Input::OnKeyEvent(e.keycode, false);
                                           return false;
                                         });
  dispatcher.Dispatch<Event::KeyRepeat>(Event::EventType::KEY_REPEAT,
                                        [this](Event::KeyRepeat &e) {
                                          // TODO make key repeat input
                                          return false;
                                        });
  // Mouse events
  dispatcher.Dispatch<Event::MouseButtonPress>(
      Event::EventType::MOUSE_BUTTON_PRESS, [this](Event::MouseButtonPress &e) {
        Input::OnMouseButtonEvent(e.keycode, true);
        return false;
      });
  dispatcher.Dispatch<Event::MouseButtonRelease>(
      Event::EventType::MOUSE_BUTTON_RELEASE,
      [this](Event::MouseButtonRelease &e) {
        Input::OnMouseButtonEvent(e.keycode, false);
        return false;
      });
  dispatcher.Dispatch<Event::MouseMove>(Event::EventType::MOUSE_MOVE,
                                        [this](Event::MouseMove &e) {
                                          Input::OnMouseMoveEvent(e.x, e.y);
                                          return false;
                                        });

  mLayerStack->DispatchEvent(event);
}
} // namespace Ember
