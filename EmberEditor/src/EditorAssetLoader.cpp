#include "EditorAssetLoader.h"

namespace Ember {

std::unordered_map<EditorIcon, AssetHandle> EditorAssetLoader::sIcons;

void EditorAssetLoader::LoadEditorAssets(AssetManager &assetManager) {

  TextureLoader::ImportSettings importSettings;
  importSettings.textureType = TextureLoader::ImportSettings::TextureType::UI;
  importSettings.filterType = TextureLoader::ImportSettings::FilterType::Linear;
  importSettings.wrapType =
      TextureLoader::ImportSettings::WrapType::ClampToEdge;
  importSettings.generateMipmaps = false;

  auto iconRoot = std::filesystem::path("../../EmberEditor/res/icons/");
  for (const auto &desc : sEditorIcons) {
    const GUID guid = EditorIconGUID(desc.icon);
    const std::filesystem::path path = iconRoot / desc.filename;

    AssetHandle handle(AssetType::TEXTURE, guid, path);
    AssetMeta meta(handle, importSettings);

    assetManager.LoadTexture(meta);
    sIcons.emplace(desc.icon, handle);
  }
}
} // namespace Ember
