#pragma once

#include "EditorAssetLoader.h"
#include "EditorPanels/IEditorPanel.h"

namespace Ember {
class MaterialEditorPanel : public IEditorPanel {
public:
  MaterialEditorPanel(EditorContext &ctx) : IEditorPanel(ctx) {}

  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  bool handleTexture(const std::string &name, GUID &guid);
};
} // namespace Ember
