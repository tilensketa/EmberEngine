#pragma once

#include <Ember.h>

namespace Ember {
enum class EditorIcon : uint8_t {
  Folder,
  FileUnknown,
  FileModel,
  FileMaterial,
  FileTexture,
  FileProject,
  FileScene,
  NullTexture
};

inline GUID EditorIconGUID(EditorIcon icon) {
  return GUID(100 + static_cast<int>(icon));
}
struct EditorIconDesc {
  EditorIcon icon;
  const char *filename;
};

static constexpr EditorIconDesc sEditorIcons[] = {
    { EditorIcon::Folder,       "folder.png" },
    { EditorIcon::FileUnknown,  "unknown.png" },
    { EditorIcon::FileModel,    "obj.png" },
    { EditorIcon::FileMaterial, "mat.png" },
    { EditorIcon::FileTexture,  "png.png" },
    { EditorIcon::FileProject,  "project.png" },
    { EditorIcon::FileScene,    "scene.png" },
    { EditorIcon::NullTexture,  "null_texture.jpg" }
};

class EditorAssetLoader {
public:
  static void LoadEditorAssets(AssetManager &assetManager);

  static const AssetHandle &GetIcon(EditorIcon icon) { return sIcons.at(icon); }

private:
  static std::unordered_map<EditorIcon, AssetHandle> sIcons;
};
} // namespace Ember
