#include "AssetDatabase.h"

#include "Core/Logger.h"

namespace Ember {
void AssetDatabase::RegisterAssetMeta(const AssetMeta &assetMeta) {
  auto [itGuid, insertedGuid] =
      mGuidToAssetMeta.emplace(assetMeta.handle.guid, assetMeta);
  if (!insertedGuid) {
    LOG_WARN("RegisterAssetMeta: GUID {} already exists. Overwriting.",
             assetMeta.handle.guid);
    itGuid->second = assetMeta;
  }

  auto [itPath, insertedPath] =
      mPathToAssetMeta.emplace(assetMeta.handle.path, assetMeta);
  if (!insertedPath) {
    LOG_WARN("RegisterAssetMeta: Path {} already exists. Overwriting.",
             assetMeta.handle.path.string());
    itPath->second = assetMeta;
  }
}
void AssetDatabase::RemoveAssetMeta(const GUID &guid) {
  auto itGuid = mGuidToAssetMeta.find(guid);
  if (itGuid != mGuidToAssetMeta.end()) mGuidToAssetMeta.erase(itGuid);

  auto itPath = mPathToAssetMeta.find(itGuid->second.handle.path);
  if (itPath != mPathToAssetMeta.end()) mPathToAssetMeta.erase(itPath);
}

std::optional<AssetMeta>
AssetDatabase::GetAssetMetaByGuid(const GUID &guid) const {
  auto it = mGuidToAssetMeta.find(guid);
  if (it != mGuidToAssetMeta.end()) return it->second;
  return std::nullopt;
}

std::optional<AssetMeta>
AssetDatabase::GetAssetMetaByPath(const std::filesystem::path &path) const {
  auto it = mPathToAssetMeta.find(path);
  if (it != mPathToAssetMeta.end()) return it->second;
  return std::nullopt;
}

std::vector<AssetHandle> AssetDatabase::GetAllAssetHandles() const {
  std::vector<AssetHandle> result;
  result.reserve(mGuidToAssetMeta.size());
  for (auto [guid, meta] : mGuidToAssetMeta) {
    result.push_back(meta.handle);
  }
  return result;
}
std::vector<AssetHandle>
AssetDatabase::GetAssetHandlesByType(const AssetType &type) const {
  std::vector<AssetHandle> result;
  result.reserve(mGuidToAssetMeta.size());
  for (auto [guid, meta] : mGuidToAssetMeta) {
    if (meta.handle.type == type) result.push_back(meta.handle);
  }
  return result;
}
} // namespace Ember
