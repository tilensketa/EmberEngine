#pragma once

#include <Ember.h>

#include "EditorAssetLoader.h"
#include "EditorPanels/IEditorPanel.h"
#include "EditorState.h"

#include <memory>

namespace Ember {

class EditorLayer : public ILayer {
public:
  EditorLayer(ProjectManager &projectManager, std::shared_ptr<Window> window,
              AssetRegistry &assetRegistry);
  ~EditorLayer();

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  void renderDockspace();
  void handleMainMenu();
  void showSceneCreateWindow();
  void configureCustomTheme();

private:
  ProjectManager &mProjectManager;
  std::array<std::shared_ptr<IEditorPanel>, 9> mEditorPanels;
  std::unique_ptr<EditorState> mEditorState = nullptr;
  std::unique_ptr<AssetManager> mAssetManager;
  AssetRegistry &mAssetRegistry;
  std::unique_ptr<EditorContext> mEditorContext;

  bool mRequestSceneCreatePopup = false;
};
} // namespace Ember
