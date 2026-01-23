#pragma once

#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class AssetRegistryPanel : public IEditorPanel {
public:
  AssetRegistryPanel(EditorContext& ctx) : IEditorPanel(ctx) {}

  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;
};
} // namespace Ember
