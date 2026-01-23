#pragma once

#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class SceneViewPanel : public IEditorPanel {
public:
  SceneViewPanel(EditorContext &ctx) : IEditorPanel(ctx) {
    mRenderTarget = std::make_unique<RenderTarget>(1000,1000);
  }
  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  void showGizmo();
  void showViewportTools();
  void handleEditor();

private:
  std::unique_ptr<RenderTarget> mRenderTarget = nullptr;
};
} // namespace Ember
