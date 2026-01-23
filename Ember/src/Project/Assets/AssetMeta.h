#pragma once

#include "AssetHandle.h"
#include "CPU/Loaders/MaterialLoader.h"
#include "CPU/Loaders/ModelLoader.h"
#include "CPU/Loaders/TextureLoader.h"

#include <variant>

#include <yaml-cpp/yaml.h>

namespace Ember {
struct AssetMeta {
  AssetHandle handle;
  std::variant<TextureLoader::ImportSettings, MaterialLoader::ImportSettings,
               ModelLoader::ImportSettings>
      importSettings;
  std::vector<GUID> dependencies;
};
} // namespace Ember

// YAML serialization
namespace YAML {
template <> struct convert<Ember::AssetMeta> {
  static Node encode(const Ember::AssetMeta &meta) {
    Node node;
    node["asset_handle"] = meta.handle;
    switch (meta.handle.type) {
    case Ember::AssetType::TEXTURE: {
      node["import_settings"] =
          std::get<Ember::TextureLoader::ImportSettings>(meta.importSettings);
      break;
    }
    case Ember::AssetType::MATERIAL: {
      node["import_settings"] =
          std::get<Ember::MaterialLoader::ImportSettings>(meta.importSettings);
      break;
    }
    case Ember::AssetType::MODEL: {
      node["import_settings"] =
          std::get<Ember::ModelLoader::ImportSettings>(meta.importSettings);
      break;
    }
    default: {
      node["import_settings"];
      break;
    }
    }
    for (const Ember::GUID &guid : meta.dependencies) {
      node["dependencies"].push_back(guid);
    }
    return node;
  }
  static bool decode(const Node &node, Ember::AssetMeta &meta) {
    meta.handle = node["asset_handle"].as<Ember::AssetHandle>();
    switch (meta.handle.type) {
    case Ember::AssetType::TEXTURE: {
      meta.importSettings =
          node["import_settings"].as<Ember::TextureLoader::ImportSettings>();
      break;
    }
    case Ember::AssetType::MATERIAL: {
      meta.importSettings =
          node["import_settings"].as<Ember::MaterialLoader::ImportSettings>();
      break;
    }
    case Ember::AssetType::MODEL: {
      meta.importSettings =
          node["import_settings"].as<Ember::ModelLoader::ImportSettings>();
      break;
    }
    default: {
      break;
    }
    }
    for (auto guidNode : node["dependencies"]) {
      meta.dependencies.push_back(guidNode.as<Ember::GUID>());
    }
    return true;
  }
};
} // namespace YAML

// Formater
template <> struct fmt::formatter<Ember::AssetMeta> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Ember::AssetMeta &m, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "(handle:{})", m.handle);
  }
};
