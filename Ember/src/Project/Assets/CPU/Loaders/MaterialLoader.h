#pragma once

#include "../../Assets.h"
#include "Core/GUID.h"
#include "Core/Logger.h"
#include "Core/Serialization.h"

#include <filesystem>
#include <fstream>

namespace Ember {
namespace MaterialLoader {

struct ImportSettings {};

inline std::optional<Asset::Material>
FromFile(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) return std::nullopt;

  auto mat = Asset::Material();
  YAML::Node yaml = YAML::LoadFile(path);
  mat.factors.albedoColor = yaml["albedo_color"].as<glm::vec3>();
  mat.factors.emission = yaml["emission"].as<float>();
  mat.factors.metalness = yaml["metalness"].as<float>();
  mat.factors.roughness = yaml["roughness"].as<float>();
  mat.albedoMap = yaml["albedo_map"].as<GUID>();
  mat.normalMap = yaml["normal_map"].as<GUID>();
  mat.metalnessMap = yaml["metalness_map"].as<GUID>();
  mat.roughnessMap = yaml["roughness_map"].as<GUID>();
  return mat;
}
inline void ToFile(const Asset::Material &mat,
                   const std::filesystem::path &path) {
  YAML::Node yaml;
  yaml["albedo_color"] = mat.factors.albedoColor;
  yaml["emission"] = mat.factors.emission;
  yaml["metalness"] = mat.factors.metalness;
  yaml["roughness"] = mat.factors.roughness;
  yaml["albedo_map"] = mat.albedoMap;
  yaml["normal_map"] = mat.normalMap;
  yaml["metalness_map"] = mat.metalnessMap;
  yaml["roughness_map"] = mat.roughnessMap;

  std::ofstream(path) << yaml;
}
} // namespace MaterialLoader
} // namespace Ember

// YAML serialization
namespace YAML {
template <> struct convert<Ember::MaterialLoader::ImportSettings> {
  static Node
  encode(const Ember::MaterialLoader::ImportSettings &importSettings) {
    Node node;
    return node;
  }
  static bool decode(const Node &node,
                     Ember::MaterialLoader::ImportSettings &importSettings) {
    return true;
  }
};
} // namespace YAML
