#pragma once

#include "AssetDatabase.h"
#include "AssetMeta.h"
#include "AssetPool.h"

namespace Ember {
class AssetManager {
public:
  // TODO all managers don't need asset root
  AssetManager(const std::filesystem::path &assetRoot);

  AssetPool &GetAssetPool() { return mAssetPool; }
  AssetDatabase &GetDatabase() { return mDatabase; }

  std::filesystem::path GetAssetsRoot() const { return mAssetsRoot; }

  std::optional<AssetHandle> LoadTexture(const AssetMeta &meta);
  std::optional<AssetHandle> LoadModel(const AssetMeta &meta);
  std::optional<AssetHandle> LoadMaterial(const AssetMeta &meta);

  // Database callers
  void RegisterAssetMeta(const AssetMeta &assetMeta);
  std::optional<AssetMeta> GetAssetMetaByGuid(const GUID &guid) const;
  std::optional<AssetMeta>
  GetAssetMetaByPath(const std::filesystem::path &path) const;

  std::vector<AssetHandle> GetAllAssetHandles() const;
  std::vector<AssetHandle> GetAssetHandlesByType(const AssetType &type) const;

  template <typename T> std::shared_ptr<T> GetAsset(const GUID &guid) {
    return mAssetPool.GetAsset<T>(guid);
  }
  void RemoveAsset(const GUID &guid);

private:
  AssetPool mAssetPool;
  AssetDatabase mDatabase;

  std::filesystem::path mAssetsRoot;
};
} // namespace Ember
