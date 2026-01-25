#pragma once

#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class TopBarPanel : public IEditorPanel {
public:
  TopBarPanel(EditorContext& ctx) : IEditorPanel(ctx) {}

  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;
};
} // namespace Ember
