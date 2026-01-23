#include "AssetRegistry.h"

namespace Ember {
void AssetRegistry::RegisterDomain(AssetDomain domain,
                                   AssetManager *assetManager) {
  LOG_DEBUG("AssetRegister: Registring domain {}", domain);
  if (!assetManager) {
    LOG_ERROR("AssetRegistry::RegisterDomain: AssetManager is null");
    return;
  }
  auto [it, inserted] = mAssetManagers.emplace(domain, assetManager);
  if (!inserted) {
    LOG_WARN("AssetRegistry: Domain {} already registered. Overwriting.",
             domain);
    it->second = assetManager;
  }
}
void AssetRegistry::UnregisterDomain(AssetDomain domain) {
  LOG_DEBUG("AssetRegister: Unregistring domain {}", domain);
  auto it = mAssetManagers.find(domain);
  if (it == mAssetManagers.end()) {
    LOG_WARN("AssetRegistry: Domain {} wasn't registered.", domain);
    return;
  }
  mAssetManagers.erase(it);
}

AssetManager *AssetRegistry::GetAssetManagerByDomain(AssetDomain domain) {
  auto it = mAssetManagers.find(domain);
  if (it != mAssetManagers.end()) return it->second;
  return nullptr;
}

std::optional<AssetHandle>
AssetRegistry::GetAssetHandleByGuid(const GUID &guid) {
  for (const AssetDomain &domain : mLookupOrder) {
    auto it = mAssetManagers.find(domain);
    if (it != mAssetManagers.end()) {
      if (auto meta = it->second->GetAssetMetaByGuid(guid)) return meta->handle;
    }
  }
  return std::nullopt;
}
std::optional<AssetHandle>
AssetRegistry::GetAssetHandleByPath(const std::filesystem::path &path) {
  for (const AssetDomain &domain : mLookupOrder) {
    auto it = mAssetManagers.find(domain);
    if (it != mAssetManagers.end()) {
      if (auto meta = it->second->GetAssetMetaByPath(path)) return meta->handle;
    }
  }
  return std::nullopt;
}

} // namespace Ember
