#pragma once

#include "ILayer.h"
#include "Project/ProjectManager.h"
#include "Renderer/Renderer.h"

#include <memory>

namespace Ember {
class RenderLayer : public ILayer {
public:
  RenderLayer(ProjectManager &projectManager,
              AssetRegistry &assetRegistry);
  ~RenderLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate() override;
  virtual void OnEvent(Event::IEvent &event) override;

private:
  std::unique_ptr<Renderer> mRenderer;
  RenderContext mRenderContext;
};
} // namespace Ember
