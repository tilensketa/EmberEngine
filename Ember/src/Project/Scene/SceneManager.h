#pragma once

#include "Core/GUID.h"
#include "Scene.h"

#include <unordered_map>

namespace Ember {
class SceneManager {
public:
  SceneManager() = default;
  ~SceneManager() = default;

  void LoadFromProject(const std::filesystem::path& scenesFolder);
  void SaveToProject(const std::filesystem::path& scenesFolder);

  GUID CreateScene(const std::string& name);
  bool RemoveScene(const GUID &guid);
  std::shared_ptr<Scene> GetCurrentScene() {
    return (mCurrentSceneGUID) ? mScenes[*mCurrentSceneGUID] : nullptr;
  }
  void ChangeScene(const GUID &guid);
  GUID DuplicateCurrentScene();

private:
  std::optional<GUID> mCurrentSceneGUID = std::nullopt;
  std::unordered_map<GUID, std::shared_ptr<Scene>> mScenes;
};
} // namespace Ember
