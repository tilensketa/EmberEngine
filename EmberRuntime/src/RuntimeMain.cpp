#include <Ember.h>
#include <EmberEntrypoint.h>

#include "GameLayer.h"

namespace Ember {
class EmberRuntime : public Application {
public:
  EmberRuntime() : Application() {
    mLayerStack->PushLayer(std::make_shared<GameLayer>(*mProjectManager, mWindow));
    mProjectManager->Open("../../Example_Project");
  }
};
Application *CreateApplication() { return new EmberRuntime(); }
} // namespace Ember
