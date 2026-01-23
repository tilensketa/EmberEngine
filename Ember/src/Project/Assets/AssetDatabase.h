#pragma once

#include "AssetMeta.h"

namespace Ember {
class AssetDatabase {
public:
  AssetDatabase() = default;

  void RegisterAssetMeta(const AssetMeta &assetMeta);
  void RemoveAssetMeta(const GUID& guid);

  std::optional<AssetMeta> GetAssetMetaByGuid(const GUID &guid) const;
  std::optional<AssetMeta>
  GetAssetMetaByPath(const std::filesystem::path &path) const;

  std::vector<AssetHandle> GetAllAssetHandles() const;
  std::vector<AssetHandle> GetAssetHandlesByType(const AssetType &type) const;

private:
  std::unordered_map<GUID, AssetMeta> mGuidToAssetMeta;
  std::unordered_map<std::filesystem::path, AssetMeta> mPathToAssetMeta;
};
} // namespace Ember
