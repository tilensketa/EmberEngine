#pragma once

#include "Layers/LayerStack.h"
#include "Layers/RenderLayer.h"
#include "Project/Assets/AssetRegistry.h"
#include "Project/ProjectManager.h"
#include "Window.h"

#include <memory>
#include <string>

namespace Ember {
struct ApplicationSpecification {
  std::string name = "DefaultApplication";
  WindowSpecification windowSpec;
};

class Application {
public:
  Application(const ApplicationSpecification &specification =
                  ApplicationSpecification());
  ~Application();

  static Application &Get();
  void DispatchEvent(Event::IEvent& event);

  void Run();
  void Stop();

protected:
  std::shared_ptr<Window> mWindow;
  std::unique_ptr<ProjectManager> mProjectManager;
  std::shared_ptr<LayerStack> mLayerStack;
  std::shared_ptr<RenderLayer> mRenderLayer;
  std::unique_ptr<AssetManager> mAssetManager;
  AssetRegistry mAssetRegistry;

private:
  static Application* sInstance;
};
Application *CreateApplication();
} // namespace Ember
