#pragma once

#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class HierarchyPanel : public IEditorPanel {
public:
  HierarchyPanel(EditorContext& ctx) : IEditorPanel(ctx) {}

  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  void handleAddMenu(std::shared_ptr<Scene> scene);
};
} // namespace Ember
