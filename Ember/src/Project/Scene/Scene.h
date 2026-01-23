#pragma once

#include "Components.h"
#include "ComponentsSerialization.h"
#include "Core/GUID.h"
#include "Core/Logger.h"
#include "Core/Serialization.h"

#include <entt/entt.hpp>

namespace Ember {
struct EntityHandle {
  entt::entity entity = entt::null;
  GUID guid = GUID::NONE();

  friend bool operator==(const EntityHandle &a, const EntityHandle &b) {
    return a.entity == b.entity && a.guid == b.guid;
  }
};

class Scene {
public:
  Scene();
  ~Scene() = default;

  static std::shared_ptr<Scene> Deserialize(const YAML::Node &node);
  YAML::Node Serialize();

  template <typename Tuple, std::size_t Index = 0>
  void DeserializeComponents(const YAML::Node &node, entt::entity entity) {
    if constexpr (Index < std::tuple_size_v<Tuple>) {
      using ComponentType = std::tuple_element_t<Index, Tuple>;
      std::string name = Component::ComponentName<ComponentType>();

      if (node[name]) {
        auto &component = mRegistry->emplace<ComponentType>(entity);
        component = node[name].as<ComponentType>();
      }
      DeserializeComponents<Tuple, Index + 1>(node, entity);
    }
  }
  template <typename Tuple, std::size_t Index = 0>
  void SerializeComponents(YAML::Node &node, const EntityHandle &handle) {
    if constexpr (Index < std::tuple_size_v<Tuple>) {
      using ComponentType = std::tuple_element_t<Index, Tuple>;
      std::string name = Component::ComponentName<ComponentType>();

      if (HasComponent<ComponentType>(handle)) {
        ComponentType &component = Get<ComponentType>(handle);
        node[name] = component;
      }
      SerializeComponents<Tuple, Index + 1>(node, handle);
    }
  }

  GUID GetGuid() const { return mGuid; }
  void SetGuid(GUID guid) { mGuid = guid; }
  std::string GetName() const { return mName; }
  void SetName(const std::string &name) { mName = name; }

  EntityHandle CreateEntity(const std::string &tag = "Default");
  void RemoveEntity(const EntityHandle &handle);

  template <typename T> bool HasComponent(const EntityHandle &handle) {
    return mRegistry->any_of<T>(handle.entity);
  }

  template <typename T> T &Get(const EntityHandle &handle) {
    return mRegistry->get<T>(handle.entity);
  }

  template <typename... Components> std::vector<EntityHandle> View() {
    std::vector<EntityHandle> result;
    auto view = mRegistry->view<Components...>();
    for (auto entity : view) {
      auto it = mEntityGuids.find(entity);
      if (it != mEntityGuids.end()) {
        result.push_back(EntityHandle({entity, it->second}));
      }
    }
    return result;
  }

  template <typename T> void AddComponent(const EntityHandle &handle, T data) {
    LOG_DEBUG("Scene::AddComponent: EntityHandle: {}, Component: {}", handle,
              Component::ComponentName<T>());
    if (HasComponent<T>(handle)) {
      LOG_ERROR("Scene::AddComponent: Entity already has component");
      return;
    }
    mRegistry->emplace<T>(handle.entity, data);
  }
  template <typename T> void AddComponent(const EntityHandle &handle) {
    AddComponent<T>(handle, T());
  }

  template <typename T> void RemoveComponent(const EntityHandle &handle) {
    LOG_DEBUG("Scene::RemoveComponent: EntityHandle: {}, Component: {}", handle,
              Component::ComponentName<T>());
    if (!HasComponent<T>(handle)) {
      LOG_ERROR("Scene::RemoveComponent: Entity doesn't have component");
      return;
    }
    mRegistry->remove<T>(handle.entity);
  }

private:
  GUID mGuid = GUID::NONE();
  std::string mName = "default_scene";
  std::unique_ptr<entt::registry> mRegistry;
  std::unordered_map<entt::entity, GUID> mEntityGuids;
};
} // namespace Ember

template <> struct fmt::formatter<entt::entity> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const entt::entity &e, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}", static_cast<entt::id_type>(e));
  }
};

template <> struct fmt::formatter<Ember::EntityHandle> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Ember::EntityHandle &h, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "({}-{})", h.entity, h.guid);
  }
};
