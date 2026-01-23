#pragma once

#include "Assets/AssetDatabase.h"
#include "Assets/AssetImporter.h"
#include "Assets/AssetManager.h"
#include "Assets/AssetRegistry.h"
#include "Scene/SceneManager.h"

#include <memory>

namespace Ember {

struct ProjectDirectories {
  std::string projectName;
  std::filesystem::path root;
  std::filesystem::path assets;
  std::filesystem::path scenes;
  std::filesystem::path library;
};

class ProjectManager {
public:
  ProjectManager(AssetRegistry &assetRegistry);
  ~ProjectManager() = default;

  void Create(const std::filesystem::path &folderPath);
  void Save();
  void Open(const std::filesystem::path &folderPath);
  void Close();

  SceneManager &GetSceneManager() { return *mSceneManager; }
  AssetManager &GetAssetManager() { return *mAssetManager; }
  AssetImporter &GetAssetImporter() { return *mAssetImporter; }
  ProjectDirectories GetProjectDirectories() const { return mDirectories; }

private:
  std::filesystem::path getProjectAbsoluteFile();

private:
  std::unique_ptr<AssetManager> mAssetManager = nullptr;
  std::unique_ptr<SceneManager> mSceneManager = nullptr;
  std::unique_ptr<AssetImporter> mAssetImporter = nullptr;

  AssetRegistry &mAssetRegistry;

  ProjectDirectories mDirectories;
};
} // namespace Ember
