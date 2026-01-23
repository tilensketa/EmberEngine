#pragma once

#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <yaml-cpp/yaml.h>

namespace YAML {

template <> struct convert<glm::uvec2> {
  static Node encode(const glm::uvec2 &vec) {
    Node node;
    node.push_back(vec.x);
    node.push_back(vec.y);
    return node;
  }
  static bool decode(const Node &node, glm::uvec2 &vec) {
    vec.x = node[0].as<unsigned int>();
    vec.y = node[1].as<unsigned int>();
    return true;
  }
};
template <> struct convert<glm::vec2> {
  static Node encode(const glm::vec2 &vec) {
    Node node;
    node.push_back(vec.x);
    node.push_back(vec.y);
    return node;
  }
  static bool decode(const Node &node, glm::vec2 &vec) {
    vec.x = node[0].as<double>();
    vec.y = node[1].as<double>();
    return true;
  }
};
template <> struct convert<glm::vec3> {
  static Node encode(const glm::vec3 &vec) {
    Node node;
    node.push_back(vec.x);
    node.push_back(vec.y);
    node.push_back(vec.z);
    return node;
  }
  static bool decode(const Node &node, glm::vec3 &vec) {
    vec.x = node[0].as<double>();
    vec.y = node[1].as<double>();
    vec.z = node[2].as<double>();
    return true;
  }
};

template <> struct convert<glm::quat> {
  static Node encode(const glm::quat &quat) {
    Node node;
    node.push_back(quat.x);
    node.push_back(quat.y);
    node.push_back(quat.z);
    node.push_back(quat.w);
    return node;
  }
  static bool decode(const Node &node, glm::quat &quat) {
    quat.x = node[0].as<double>();
    quat.y = node[1].as<double>();
    quat.z = node[2].as<double>();
    quat.w = node[3].as<double>();
    return true;
  }
};
} // namespace YAML
