#pragma once

#include "AssetType.h"
#include "Core/GUID.h"

#include <filesystem>

#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

namespace Ember {

struct AssetHandle {
  AssetType type;
  GUID guid = GUID::NONE();
  std::filesystem::path path;

  AssetHandle() = default;
  AssetHandle(const AssetType &assetType, const GUID &guid,
              const std::filesystem::path &path)
      : type(assetType), guid(guid), path(path) {}

  static AssetHandle NONE() {
    AssetHandle handle;
    handle.type = AssetType::NONE;
    handle.guid = GUID::NONE();
    handle.path = std::filesystem::path();
    return handle;
  }

  friend bool operator==(const AssetHandle &a, const AssetHandle &b) {
    return a.type == b.type && a.guid == b.guid && a.path == b.path;
  }
};
} // namespace Ember

// YAML serialization
namespace YAML {
template <> struct convert<Ember::AssetHandle> {
  static Node encode(const Ember::AssetHandle &handle) {
    Node node;
    node["type"] = handle.type;
    node["guid"] = handle.guid;
    node["path"] = handle.path.string();
    return node;
  }

  static bool decode(const Node &node, Ember::AssetHandle &handle) {
    handle.type = node["type"].as<Ember::AssetType>();
    handle.guid = node["guid"].as<Ember::GUID>();
    handle.path = node["path"].as<std::string>();
    return true;
  }
};
} // namespace YAML

// Formater
template <> struct fmt::formatter<Ember::AssetHandle> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Ember::AssetHandle &h, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "({}-{}-{})", h.type, h.guid,
                          h.path.c_str());
  }
};
