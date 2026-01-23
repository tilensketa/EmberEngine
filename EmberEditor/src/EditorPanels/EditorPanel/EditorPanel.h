#pragma once

#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class EditorPanel : public IEditorPanel {
public:
  EditorPanel(EditorContext& ctx) : IEditorPanel(ctx) {}
  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;
};
} // namespace Ember
