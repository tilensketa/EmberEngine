#include "SceneManager.h"
#include "Core/Logger.h"
#include "Core/Serialization.h"
#include "Project/FileTypes.h"

namespace Ember {

void SceneManager::LoadFromProject(const std::filesystem::path &scenesFolder) {
  LOG_INFO("SceneManager::LoadFromProject: folder: {}", scenesFolder.string());
  for (auto &entry : std::filesystem::directory_iterator(scenesFolder)) {
    if (FileTypeUtils::FromPath(entry.path()) != FileType::Scene) continue;

    LOG_INFO("SceneManager::LoadFromProject: scene: {}", entry.path().string());
    YAML::Node root = YAML::LoadFile(entry.path().string());
    auto scene = Scene::Deserialize(root);

    mScenes[scene->GetGuid()] = scene;
  }
}
void SceneManager::SaveToProject(const std::filesystem::path &scenesFolder) {
  LOG_INFO("SceneManager::SaveToProject: main folder: {}",
           scenesFolder.string());
  for (const auto &[guid, scene] : mScenes) {
    std::string filename =
        scene->GetName() + FileTypeUtils::ToPath(FileType::Scene);
    auto path = scenesFolder / filename;
    LOG_INFO("SceneManager::SaveToProject: scene folder: {}", path.string());
    YAML::Node node = scene->Serialize();
    std::ofstream(path) << node;
  }
}
GUID SceneManager::CreateScene(const std::string& name) {
  GUID sceneGuid = GUID();

  LOG_DEBUG("SceneManager::CreateScene: Creating scene with guid: {}",
            sceneGuid);
  auto scene = std::make_shared<Scene>();
  scene->SetName(name);
  scene->SetGuid(sceneGuid);
  mScenes.insert({sceneGuid, scene});
  mCurrentSceneGUID = sceneGuid;
  return sceneGuid;
}
bool SceneManager::RemoveScene(const GUID &guid) {
  LOG_INFO("SceneManager::RemoveScene: Removing scene with guid: {}", guid);
  mCurrentSceneGUID = std::nullopt;
  return mScenes.erase(guid) > 0;
}
void SceneManager::ChangeScene(const GUID &guid) {
  if (guid == GUID::NONE()) {
    LOG_WARN(
        "SceneManager::ChangeScene: Trying to change scene with GUID::NONE()");
    return;
  }
  auto it = mScenes.find(guid);
  if (it != mScenes.end()) mCurrentSceneGUID = guid;
}
std::vector<GUID> SceneManager::GetSceneGUIDs() const {
  std::vector<GUID> sceneGuids;
  sceneGuids.reserve(mScenes.size());
  for (const auto &[guid, scene] : mScenes) {
    sceneGuids.push_back(guid);
  }
  return sceneGuids;
}

} // namespace Ember
