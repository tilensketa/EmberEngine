#pragma once

#include "../CPUAssets.h"

#include <optional>

#include <yaml-cpp/yaml.h>

namespace Ember {
namespace ModelLoader {
struct ImportSettings {};

CPUAsset::ModelData CreateCube();
CPUAsset::ModelData CreateSphere();
CPUAsset::ModelData CreateCylinder();

std::optional<CPUAsset::ModelData> FromFile(const std::filesystem::path &path);

} // namespace ModelLoader
} // namespace Ember

// YAML serialization
namespace YAML {
template <> struct convert<Ember::ModelLoader::ImportSettings> {
  static Node encode(const Ember::ModelLoader::ImportSettings &importSettings) {
    Node node;
    return node;
  }
  static bool decode(const Node &node,
                     Ember::ModelLoader::ImportSettings &importSettings) {
    return true;
  }
};
} // namespace YAML
