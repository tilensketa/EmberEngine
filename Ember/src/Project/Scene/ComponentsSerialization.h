#pragma once

#include "Core/Utils.h"
#include "Project/Scene/Components.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {
template <> struct convert<Ember::Component::Id> {
  static Node encode(const Ember::Component::Id &id) {
    Node node;
    node = id.guid;
    return node;
  }
  static bool decode(const Node &node, Ember::Component::Id &id) {
    id.guid = node.as<Ember::GUID>();
    return true;
  }
};

template <> struct convert<Ember::Component::Tag> {
  static Node encode(const Ember::Component::Tag &tag) {
    Node node;
    node = tag.value;
    return node;
  }
  static bool decode(const Node &node, Ember::Component::Tag &tag) {
    tag.value = node.as<std::string>();
    return true;
  }
};

template <> struct convert<Ember::Component::Transform> {
  static Node encode(const Ember::Component::Transform &transform) {
    Node node;
    node["position"] = transform.position;
    node["rotation"] = transform.rotation;
    node["scale"] = transform.scale;
    return node;
  }
  static bool decode(const Node &node, Ember::Component::Transform &transform) {
    transform.position = node["position"].as<glm::vec3>();
    transform.rotation = node["rotation"].as<glm::quat>();
    transform.scale = node["scale"].as<glm::vec3>();
    return true;
  }
};

template <> struct convert<Ember::Component::Model> {
  static Node encode(const Ember::Component::Model &model) {
    Node node;
    if (model.model) node["model"] = *model.model;
    Node materialOverridesNode;
    for (const Ember::GUID &guid : model.materialOverrides) {
      materialOverridesNode.push_back(guid);
    }
    node["material_overrides"] = materialOverridesNode;
    return node;
  }
  static bool decode(const Node &node, Ember::Component::Model &model) {
    if (node["model"])
      model.model = node["model"].as<Ember::AssetHandle>();
    else
      model.model = std::nullopt;

    model.materialOverrides.clear();
    if (node["material_overrides"]) {
      for (const auto &item : node["material_overrides"]) {
        model.materialOverrides.push_back(item.as<Ember::GUID>());
      }
    }
    return true;
  }
};

template <> struct convert<Ember::Component::Camera> {
  static Node encode(const Ember::Component::Camera &camera) {
    Node node;
    node["projection"] =
        Ember::Utils::ToString<Ember::Component::Camera::Projection>(
            camera.projection);
    node["view_mode"] =
        Ember::Utils::ToString<Ember::Component::Camera::ViewMode>(
            camera.viewMode);
    node["resolution"] = camera.resolution;
    node["ortho_size"] = camera.orthoSize;
    node["near"] = camera.near;
    node["far"] = camera.far;
    node["fov"] = camera.fov;
    node["active"] = camera.active;
    return node;
  }
  static bool decode(const Node &node, Ember::Component::Camera &camera) {
    camera.projection =
        Ember::Utils::FromString<Ember::Component::Camera::Projection>(
            node["projection"].as<std::string>());
    camera.viewMode =
        Ember::Utils::FromString<Ember::Component::Camera::ViewMode>(
            node["view_mode"].as<std::string>());
    camera.resolution = node["resolution"].as<glm::uvec2>();
    camera.orthoSize = node["ortho_size"].as<float>();
    camera.near = node["near"].as<float>();
    camera.far = node["far"].as<float>();
    camera.fov = node["fov"].as<float>();
    camera.active = node["active"].as<bool>();
    return true;
  }
};

template <> struct convert<Ember::Component::Light> {
  static Node encode(const Ember::Component::Light &light) {
    Node node;
    node["light_type"] =
        Ember::Utils::ToString<Ember::Component::Light::LightType>(
            light.lightType);
    node["color"] = light.color;
    node["intensity"] = light.intensity;
    node["radius"] = light.radius;
    return node;
  }
  static bool decode(const Node &node, Ember::Component::Light &light) {
    light.lightType =
        Ember::Utils::FromString<Ember::Component::Light::LightType>(
            node["light_type"].as<std::string>());
    light.color = node["color"].as<glm::vec3>();
    light.intensity = node["intensity"].as<float>();
    light.radius = node["radius"].as<float>();
    return true;
  }
};
} // namespace YAML
