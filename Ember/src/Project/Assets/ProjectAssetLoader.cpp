#include "ProjectAssetLoader.h"

#include "AssetMeta.h"
#include "Project/FileTypes.h"

namespace Ember {
void ProjectAssetLoader::Import(AssetManager &assetManager) {
  const std::filesystem::path assets = assetManager.GetAssetsRoot();
  if (!std::filesystem::exists(assets)) {
    LOG_CRITICAL("ProjectAssetLoader::Load: AssetRoot doesnt exist: {}",
                 assets.string());
    return;
  }

  for (auto &entry : std::filesystem::recursive_directory_iterator(assets)) {
    FileType fileType = FileTypeUtils::FromPath(entry.path());
    if (fileType == FileType::AssetMeta) {
      LOG_DEBUG("Importing AssetMeta: {}", entry.path().string());
      YAML::Node node = YAML::LoadFile(entry.path());
      AssetMeta meta = node.as<AssetMeta>();
      assetManager.GetDatabase().RegisterAssetMeta(meta);
    }
  }
}

void ProjectAssetLoader::Load(AssetManager &assetManager){
  const auto& database = assetManager.GetDatabase();
  for (auto &handle : database.GetAssetHandlesByType(AssetType::TEXTURE)) {
    auto metaOpt = database.GetAssetMetaByGuid(handle.guid);
    if (metaOpt) assetManager.LoadTexture(*metaOpt);
  }
  for (auto &handle : database.GetAssetHandlesByType(AssetType::MATERIAL)) {
    auto metaOpt = database.GetAssetMetaByGuid(handle.guid);
    if (metaOpt) assetManager.LoadMaterial(*metaOpt);
  }
  for (auto &handle : database.GetAssetHandlesByType(AssetType::MODEL)) {
    auto metaOpt = database.GetAssetMetaByGuid(handle.guid);
    if (metaOpt) assetManager.LoadModel(*metaOpt);
  }
}
} // namespace Ember
