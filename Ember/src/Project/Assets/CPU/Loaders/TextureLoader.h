#pragma once

#include "../CPUAssets.h"
#include "Core/Utils.h"

#include <stb_image.h>

#include <filesystem>
#include <optional>

#include <yaml-cpp/yaml.h>

namespace Ember {
namespace TextureLoader {
struct ImportSettings {
  enum class TextureType : uint8_t {
    Albedo = 0,
    Normal,
    Metalness,
    Roughness,
    UI,
    COUNT
  };
  enum class FilterType : uint8_t { Nearest = 0, Linear, COUNT };
  enum class WrapType : uint8_t {
    ClampToEdge = 0,
    ClampToBorder,
    MirroredRepeat,
    Repeat,
    MirrorClampToEdge,
    COUNT
  };

  TextureType textureType = TextureType::Albedo;
  FilterType filterType = FilterType::Linear;
  WrapType wrapType = WrapType::Repeat;
  bool generateMipmaps = false;
};

std::optional<CPUAsset::TextureData>
FromFile(const std::filesystem::path &filePath);
} // namespace TextureLoader
} // namespace Ember

namespace Ember::Utils {
template <> struct EnumTraits<TextureLoader::ImportSettings::TextureType> {
  inline static constexpr std::array<const char *, 5> Names = {
      "Albedo", "Normal", "Metalness", "Roughness", "UI"};
};
template <> struct EnumTraits<TextureLoader::ImportSettings::FilterType> {
  inline static constexpr std::array<const char *, 2> Names = {"Nearest",
                                                               "Linear"};
};
template <> struct EnumTraits<TextureLoader::ImportSettings::WrapType> {
  inline static constexpr std::array<const char *, 5> Names = {
      "ClampToEdge", "ClampToBorder", "MirroredRepeat", "Repeat",
      "MirrorClampToEdge"};
};
} // namespace Ember::Utils

// YAML serialization
namespace YAML {
template <> struct convert<Ember::TextureLoader::ImportSettings> {
  static Node
  encode(const Ember::TextureLoader::ImportSettings &importSettings) {
    Node node;
    node["texture_type"] = Ember::Utils::ToString<
        Ember::TextureLoader::ImportSettings::TextureType>(
        importSettings.textureType);
    node["filter_type"] = Ember::Utils::ToString<
        Ember::TextureLoader::ImportSettings::FilterType>(
        importSettings.filterType);
    node["wrap_type"] =
        Ember::Utils::ToString<Ember::TextureLoader::ImportSettings::WrapType>(
            importSettings.wrapType);
    node["generate_mipmaps"] = importSettings.generateMipmaps;
    return node;
  }
  static bool decode(const Node &node,
                     Ember::TextureLoader::ImportSettings &importSettings) {
    importSettings.textureType = Ember::Utils::FromString<
        Ember::TextureLoader::ImportSettings::TextureType>(
        node["texture_type"].as<std::string>());
    importSettings.filterType = Ember::Utils::FromString<
        Ember::TextureLoader::ImportSettings::FilterType>(
        node["filter_type"].as<std::string>());
    importSettings.wrapType = Ember::Utils::FromString<
        Ember::TextureLoader::ImportSettings::WrapType>(
        node["wrap_type"].as<std::string>());
    importSettings.generateMipmaps = node["generate_mipmaps"].as<bool>();
    return true;
  }
};
} // namespace YAML
