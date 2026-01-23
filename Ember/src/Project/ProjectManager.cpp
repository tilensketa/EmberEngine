#include "ProjectManager.h"

#include "Assets/ProjectAssetLoader.h"
#include "Core/Serialization.h"

#include "Core/Application.h"
#include "Core/Events/Events.h"
#include "Project/FileTypes.h"

#include <fstream>

namespace Ember {
ProjectManager::ProjectManager(AssetRegistry &assetRegistry)
    : mAssetRegistry(assetRegistry) {}

void ProjectManager::Create(const std::filesystem::path &folder) {
  // Creates project file and start scene file then opens it
  LOG_INFO("Creating project: {}", folder.string());
  mDirectories.projectName = folder.filename();
  mDirectories.root = folder;
  mDirectories.assets = folder / "Assets";
  mDirectories.scenes = folder / "Scenes";
  mDirectories.library = folder / "Library";

  std::filesystem::create_directories(mDirectories.root);
  std::filesystem::create_directories(mDirectories.assets);
  std::filesystem::create_directories(mDirectories.scenes);
  std::filesystem::create_directories(mDirectories.library);

  GUID startSceneGuid = GUID();
  YAML::Node startScene;
  startScene["guid"] = startSceneGuid;
  startScene["name"] = "default_scene";
  auto outputPath =
      mDirectories.scenes / (std::to_string(startSceneGuid.GetID()) +
                             FileTypeUtils::ToPath(FileType::Scene));
  std::ofstream(outputPath) << startScene;

  YAML::Node project;
  project["name"] = mDirectories.projectName;
  project["asset_root"] = "Assets";
  project["scene_root"] = "Scenes";
  project["library_root"] = "Library";
  project["start_scene"] = startSceneGuid;
  std::ofstream(getProjectAbsoluteFile()) << project;

  Event::ProjectCreate event;
  Application::Get().DispatchEvent(event);

  Open(folder);
}
void ProjectManager::Save() {
  YAML::Node project = YAML::LoadFile(getProjectAbsoluteFile());
  auto startScene = mSceneManager->GetCurrentScene();
  if (startScene) project["start_scene"] = startScene->GetGuid();

  std::ofstream(getProjectAbsoluteFile()) << project;

  mSceneManager->SaveToProject(mDirectories.scenes);

  Event::ProjectSave event;
  Application::Get().DispatchEvent(event);
}
void ProjectManager::Open(const std::filesystem::path &folder) {
  Close();

  mDirectories.projectName = folder.filename();
  mDirectories.root = folder;

  LOG_INFO("ProjectManager::Open: project name: {} / abs: {}",
           mDirectories.projectName, getProjectAbsoluteFile().string());

  YAML::Node project = YAML::LoadFile(getProjectAbsoluteFile());
  mDirectories.assets = folder / project["asset_root"].as<std::string>();
  mDirectories.scenes = folder / project["scene_root"].as<std::string>();
  mDirectories.library = folder / project["library_root"].as<std::string>();

  // Load Scene Manager
  mSceneManager = std::make_unique<SceneManager>();
  mSceneManager->LoadFromProject(mDirectories.scenes);
  const GUID startSceneGuid = project["start_scene"].as<GUID>();
  mSceneManager->ChangeScene(startSceneGuid);

  // Load Asset Manager
  mAssetManager = std::make_unique<AssetManager>(mDirectories.assets);
  mAssetImporter = std::make_unique<AssetImporter>(*mAssetManager);
  ProjectAssetLoader::Import(*mAssetManager);
  ProjectAssetLoader::Load(*mAssetManager);

  // Register Asset Manager Domain
  mAssetRegistry.RegisterDomain(AssetDomain::Project, mAssetManager.get());

  Event::ProjectOpen event(mDirectories);
  Application::Get().DispatchEvent(event);
}
void ProjectManager::Close() {
  LOG_DEBUG("ProjectManager::Close");
  mSceneManager.reset();
  mAssetRegistry.UnregisterDomain(AssetDomain::Project);
  mAssetManager.reset();

  Event::ProjectClose event;
  Application::Get().DispatchEvent(event);
}

std::filesystem::path ProjectManager::getProjectAbsoluteFile() {
  return mDirectories.root /
         (mDirectories.projectName + FileTypeUtils::ToPath(FileType::Project));
}
} // namespace Ember
