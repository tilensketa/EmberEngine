#include <Ember.h>
#include <EmberEntrypoint.h>

#include "GameLayer.h"

namespace Ember {
class EmberRuntime : public Application {
public:
  EmberRuntime() : Application() {
    mLayerStack->PushLayer(
        std::make_shared<GameLayer>(*mProjectManager, mWindow));
    mProjectManager->Open("../../Example_Project");
    Event::PhysicsStart event(
        mProjectManager->GetSceneManager().GetCurrentScene());
    Application::Get().DispatchEvent(event);
  }
};
Application *CreateApplication() { return new EmberRuntime(); }
} // namespace Ember
