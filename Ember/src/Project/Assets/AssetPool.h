#pragma once

#include "AssetHandle.h"
#include "Core/GUID.h"
#include "Core/Logger.h"

#include <unordered_map>

namespace Ember {

class AssetPool {
public:
  AssetPool() = default;
  ~AssetPool() = default;

  template <typename T> std::shared_ptr<T> GetAsset(const GUID &guid) {
    auto it = mAssets.find(guid);
    if (it == mAssets.end()) {
      return nullptr;
    }
    return std::static_pointer_cast<T>(it->second.Data);
  }

  template <typename T>
  void StoreAsset(const AssetHandle &handle, std::shared_ptr<T> asset) {
    LOG_DEBUG("AssetPool::StoreAsset: Storing asset with handle: {}", handle);
    mAssets.insert({handle.guid, AssetRecord({handle, asset})});
  }

  bool HasAsset(const GUID &guid) {
    return (mAssets.find(guid) != mAssets.end());
  }

  void RemoveAsset(const GUID &guid) {
    LOG_DEBUG("AssetPool::RemoveAsset: Removing asset with guid: {}", guid);
    auto it = mAssets.find(guid);
    if(it != mAssets.end()) mAssets.erase(it);
  }

private:
  struct AssetRecord {
    AssetHandle Handle;
    std::shared_ptr<void> Data;
  };
  std::unordered_map<GUID, AssetRecord> mAssets;
};
} // namespace Ember
