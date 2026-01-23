#pragma once

#include "AssetHandle.h"
#include "AssetManager.h"
#include "CPU/Loaders/ModelLoader.h"
#include "CPU/Loaders/TextureLoader.h"

namespace Ember {
class AssetImporter {
public:
  AssetImporter(AssetManager& assetManager);

  std::optional<AssetHandle>
  CreateMaterial(const std::filesystem::path &absPath);
  std::optional<AssetHandle>
  CreateEmbededMaterial(const std::filesystem::path &absPath,
                 CPUAsset::MaterialData cpuMat, std::vector<GUID> textureGuids);

  std::optional<AssetHandle>
  ImportTexture(const std::filesystem::path &relPath,
                const TextureLoader::ImportSettings &importSettings);
  std::optional<AssetHandle>
  ImportModel(const std::filesystem::path &relPath,
              const ModelLoader::ImportSettings &importSettings);

private:
  void writeMeta(const std::filesystem::path &relPath, const AssetMeta &meta);

private:
  AssetManager& mAssetManager;
};
} // namespace Ember
