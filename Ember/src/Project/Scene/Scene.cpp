#include "Scene.h"
#include "Project/Scene/Components.h"

namespace Ember {
Scene::Scene() {
  mGuid = GUID();
  mRegistry = std::make_unique<entt::registry>();
}

std::shared_ptr<Scene> Scene::Deserialize(const YAML::Node &node) {
  LOG_DEBUG("Scene::Deserialize");
  std::shared_ptr<Scene> scene = std::make_shared<Scene>();
  scene->mGuid = node["guid"].as<GUID>();
  scene->mName = node["name"].as<std::string>();
  for (const auto &entNode : node["entities"]) {
    entt::entity entity = scene->mRegistry->create();
    GUID guid = entNode["Id"].as<GUID>();
    scene->mEntityGuids[entity] = guid;

    scene->DeserializeComponents<Component::AllComponents>(entNode, entity);
  }
  return scene;
}
YAML::Node Scene::Serialize() {
  LOG_DEBUG("Scene::Serialize");
  YAML::Node node;
  node["guid"] = mGuid;
  node["name"] = mName;
  node["entities"] = YAML::Node(YAML::NodeType::Sequence);
  auto view = View<Component::Id>();
  for (auto entity : view) {
    YAML::Node entityYaml;
    SerializeComponents<Component::AllComponents>(entityYaml, entity);
    node["entities"].push_back(entityYaml);
  }
  return node;
}

EntityHandle Scene::CreateEntity(const std::string &tag) {
  GUID guid = GUID();
  entt::entity entity = mRegistry->create();
  mEntityGuids[entity] = guid;
  EntityHandle entityHandle = EntityHandle({entity, guid});
  LOG_DEBUG("Scene::CreateEntity: Creating entity handle: {}", entityHandle);
  AddComponent(entityHandle, Component::Id({guid}));
  AddComponent(entityHandle, Component::Tag({tag}));
  AddComponent(entityHandle, Component::Transform());
  return entityHandle;
}
void Scene::RemoveEntity(const EntityHandle &handle) {
  mRegistry->destroy(handle.entity);
  mEntityGuids.erase(handle.entity);
}
} // namespace Ember
