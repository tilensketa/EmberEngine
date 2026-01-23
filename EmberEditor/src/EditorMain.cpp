#include <Ember.h>
#include <EmberEntrypoint.h>

#include "EditorLayer.h"

#define FONTS "../../EmberEditor/res/fonts/"

namespace Ember {
class EmberEditor : public Application {
public:
  EmberEditor() : Application() {
    mLayerStack->PushLayer(std::make_shared<EditorLayer>(
        *mProjectManager, mWindow, mAssetRegistry));

    mProjectManager->Open("../../Example_Project");
  }
};
Application *CreateApplication() { return new EmberEditor(); }
} // namespace Ember
