#pragma once

#include <filesystem>
#include <string_view>
#include <unordered_map>

namespace Ember {
enum class FileType {
  Unknown,

  // External source
  ModelSource,
  TextureSource,

  // Engine-authored
  Material,
  Scene,
  Project,
  AssetMeta,

  // Engine cache
  ModelCache
};

namespace FileTypeUtils {
namespace FileTypeMap {
inline const std::unordered_map<std::string_view, FileType> ExtensionToType = {
    // External Sources
    // Models
    {".obj", FileType::ModelSource},
    {".gltf", FileType::ModelSource},
    // Textures
    {".png", FileType::TextureSource},

    // Engine files
    {".embermat", FileType::Material},
    {".emberscene", FileType::Scene},
    {".ember", FileType::Project},
    {".embermeta", FileType::AssetMeta},

    // Engine cache
    {".embermodelbin", FileType::ModelCache}};
}

static FileType FromPath(const std::filesystem::path &filename) {
  const std::string ext = filename.extension().string();

  auto it = FileTypeMap::ExtensionToType.find(ext);
  if (it != FileTypeMap::ExtensionToType.end())
    return static_cast<FileType>(it->second);
  return FileType::Unknown;
}

static std::string ToPath(const FileType &type) {
  for (const auto &[k, v] : FileTypeMap::ExtensionToType) {
    if (type == v) return std::string(k);
  }
  return ".none";
}
} // namespace FileTypeUtils
} // namespace Ember
