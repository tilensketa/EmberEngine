#pragma once

#include "Core/Utils.h"

#include <array>
#include <string>

#include <spdlog/fmt/fmt.h>
#include <yaml-cpp/yaml.h>

namespace Ember {
enum class AssetType : uint8_t { NONE = 0, MODEL, TEXTURE, MATERIAL };
} // namespace Ember

namespace Ember::Utils {
template <> struct EnumTraits<AssetType> {
  inline static constexpr std::array<const char *, 4> Names = {
      "None", "Model", "Texture", "Material"};
};
} // namespace Ember::Utils

// YAML serialization
namespace YAML {
template <> struct convert<Ember::AssetType> {
  static Node encode(const Ember::AssetType &type) {
    Node node(Ember::Utils::ToString<Ember::AssetType>(type));
    return node;
  }

  static bool decode(const Node &node, Ember::AssetType &type) {
    type = Ember::Utils::FromString<Ember::AssetType>(node.as<std::string>());
    return true;
  }
};
} // namespace YAML

// Formater
template <> struct fmt::formatter<Ember::AssetType> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Ember::AssetType &t, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}",
                          Ember::Utils::ToString<Ember::AssetType>(t));
  }
};
