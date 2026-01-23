#pragma once

#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class GameViewPanel : public IEditorPanel {
public:
  GameViewPanel(EditorContext &ctx) : IEditorPanel(ctx) {
    mRenderTarget = std::make_unique<RenderTarget>(1000,1000);
  }
  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  std::unique_ptr<RenderTarget> mRenderTarget = nullptr;
};
} // namespace Ember
