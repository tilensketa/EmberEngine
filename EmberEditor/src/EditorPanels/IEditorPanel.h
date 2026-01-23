#pragma once

#include "EditorState.h"
#include "imgui.h"
#include <Ember.h>

namespace Ember {

struct EditorContext {
  ProjectManager &projectManager;
  EditorState &editorState;
  AssetManager& assetManager;
  AssetRegistry& assetRegistry;
};

class IEditorPanel {
public:
  explicit IEditorPanel(EditorContext &ctx) : mCtx(ctx) {}
  virtual ~IEditorPanel() = default;

  virtual void Update() {};
  virtual void OnEvent(Ember::Event::IEvent &event) {};

protected:
  EditorContext &mCtx;
};
} // namespace Ember
