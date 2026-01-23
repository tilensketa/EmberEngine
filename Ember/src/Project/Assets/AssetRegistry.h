#pragma once

#include "AssetManager.h"
#include "Core/Utils.h"

#include <cstdint>
#include <vector>

namespace Ember {
enum class AssetDomain : uint8_t { Engine = 0, Project };
class AssetRegistry {
public:
  void RegisterDomain(AssetDomain domain, AssetManager *assetManager);
  void UnregisterDomain(AssetDomain domain);

  template <typename T> std::shared_ptr<T> GetAsset(const GUID &guid) {
    for (const AssetDomain &domain : mLookupOrder) {
      auto it = mAssetManagers.find(domain);
      if (it != mAssetManagers.end()) {
        if (auto asset = it->second->GetAsset<T>(guid)) return asset;
      }
    }
    return nullptr;
  }
  AssetManager *GetAssetManagerByDomain(AssetDomain domain);
  std::optional<AssetHandle> GetAssetHandleByGuid(const GUID &guid);
  std::optional<AssetHandle>
  GetAssetHandleByPath(const std::filesystem::path &path);

private:
  std::unordered_map<AssetDomain, AssetManager *> mAssetManagers;
  std::vector<AssetDomain> mLookupOrder = {AssetDomain::Project,
                                           AssetDomain::Engine};
};
} // namespace Ember

namespace Ember::Utils {
template <> struct EnumTraits<AssetDomain> {
  inline static constexpr std::array<const char *, 2> Names = {"Engine",
                                                               "Project"};
};
} // namespace Ember::Utils

// Formater
template <> struct fmt::formatter<Ember::AssetDomain> {
  constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

  template <typename FormatContext>
  auto format(const Ember::AssetDomain &d, FormatContext &ctx) const {
    return fmt::format_to(ctx.out(), "{}",
                          Ember::Utils::ToString<Ember::AssetDomain>(d));
  }
};
