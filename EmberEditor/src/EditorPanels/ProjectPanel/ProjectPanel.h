#pragma once

#include <Ember.h>

#include "EditorPanels/IEditorPanel.h"

#include "EditorAssetLoader.h"

namespace Ember {

class ProjectPanel : public IEditorPanel {
public:
  ProjectPanel(EditorContext& ctx) : IEditorPanel(ctx) {}
  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  void handleNavigationBar();
  void handleFileViewer();

  void handleAddMenu();

  EditorIcon fileTypeToEditorIcon(const FileType &type);

  void showMaterialCreateWindow();
  void showFolderCreateWindow();

  void showTextureLoadWindow();
  void showModelLoadWindow();

private:
  std::filesystem::path mPendingImportPath;
  bool mRequestModelImportPopup = false;
  bool mRequestTextureImportPopup = false;

  bool mRequestMaterialCreatePopup = false;
  bool mRequestFolderCreatePopup = false;
};
} // namespace Ember
