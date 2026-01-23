#include "AssetImporter.h"

#include "CPU/Loaders/ModelBinaryLoader.h"
#include "Project/FileTypes.h"

namespace Ember {

AssetImporter::AssetImporter(AssetManager &assetManager)
    : mAssetManager(assetManager) {}

std::optional<AssetHandle>
AssetImporter::CreateMaterial(const std::filesystem::path &absPath) {
  auto pathWithExtension =
      absPath.string() + FileTypeUtils::ToPath(FileType::Material);
  auto relativePath = std::filesystem::relative(pathWithExtension,
                                                mAssetManager.GetAssetsRoot());

  Asset::Material mat;
  MaterialLoader::ToFile(mat, pathWithExtension);

  AssetMeta meta;
  meta.handle = AssetHandle(AssetType::MATERIAL, GUID(), relativePath);
  meta.importSettings = MaterialLoader::ImportSettings();

  writeMeta(relativePath, meta);
  mAssetManager.RegisterAssetMeta(meta);

  return meta.handle;
}

std::optional<AssetHandle>
AssetImporter::CreateEmbededMaterial(const std::filesystem::path &absPath,
                                     CPUAsset::MaterialData cpuMat,
                                     std::vector<GUID> textureGuids) {
  auto pathWithExtension =
      absPath.string() + FileTypeUtils::ToPath(FileType::Material);
  auto relativePath = std::filesystem::relative(pathWithExtension,
                                                mAssetManager.GetAssetsRoot());

  Asset::Material mat;
  mat.factors = cpuMat.matFactors;
  mat.albedoMap = textureGuids[0];
  mat.normalMap = textureGuids[1];
  mat.metalnessMap = textureGuids[2];
  mat.roughnessMap = textureGuids[3];
  MaterialLoader::ToFile(mat, pathWithExtension);

  AssetMeta meta;
  meta.handle = AssetHandle(AssetType::MATERIAL, GUID(), relativePath);
  meta.importSettings = MaterialLoader::ImportSettings();
  meta.dependencies = textureGuids;

  writeMeta(relativePath, meta);
  mAssetManager.RegisterAssetMeta(meta);

  return meta.handle;
}

std::optional<AssetHandle> AssetImporter::ImportTexture(
    const std::filesystem::path &relPath,
    const TextureLoader::ImportSettings &importSettings) {
  AssetMeta meta;
  meta.handle = AssetHandle(AssetType::TEXTURE, GUID(), relPath);
  meta.importSettings = importSettings;

  writeMeta(relPath, meta);
  mAssetManager.RegisterAssetMeta(meta);

  return meta.handle;
}
std::optional<AssetHandle>
AssetImporter::ImportModel(const std::filesystem::path &relPath,
                           const ModelLoader::ImportSettings &importSettings) {
  auto absPath = mAssetManager.GetAssetsRoot() / relPath;
  auto cpuDataOpt = ModelLoader::FromFile(absPath);
  if (!cpuDataOpt) {
    LOG_ERROR("AssetImporter::ImportModel: ModelLoader::FromFile: Failed to "
              "load from path: {}",
              absPath.string());
    return std::nullopt;
  }
  const CPUAsset::ModelData cpuData = *cpuDataOpt;

  std::vector<GUID> materialGuids;
  int i = 0;
  for (auto &cpuMat : cpuData.materials) {
    std::vector<GUID> textureGuids;

    auto importTexture =
        [&](const std::filesystem::path &texPath,
            const TextureLoader::ImportSettings &importSettings) -> GUID {
      if (texPath.empty()) return GUID::NONE();
      auto relTexPath =
          std::filesystem::relative(texPath, mAssetManager.GetAssetsRoot());
      auto texMetaOpt = mAssetManager.GetAssetMetaByPath(relTexPath);
      if (texMetaOpt) return texMetaOpt->handle.guid;
      auto texHandleOpt = ImportTexture(relTexPath, importSettings);
      if (texHandleOpt) return texHandleOpt->guid;
      return GUID::NONE();
    };

    TextureLoader::ImportSettings settings;
    settings.generateMipmaps = true;

    settings.textureType = TextureLoader::ImportSettings::TextureType::Albedo;
    GUID albedo = importTexture(cpuMat.albedoMap, settings);
    settings.textureType = TextureLoader::ImportSettings::TextureType::Normal;
    GUID normal = importTexture(cpuMat.normalMap, settings);
    settings.textureType =
        TextureLoader::ImportSettings::TextureType::Metalness;
    GUID metal = importTexture(cpuMat.metalnessMap, settings);
    settings.textureType =
        TextureLoader::ImportSettings::TextureType::Roughness;
    GUID rough = importTexture(cpuMat.roughnessMap, settings);
    textureGuids = {albedo, normal, metal, rough};

    auto matPath = mAssetManager.GetAssetsRoot() /
                   (relPath.string() + "mat_" + (std::to_string(i++)));
    auto matHandleOpt = CreateEmbededMaterial(matPath, cpuMat, textureGuids);
    if (matHandleOpt) materialGuids.push_back(matHandleOpt->guid);
  }

  ModelBinaryLoader::ModelBinaryData modelBinaryData;
  for (const auto &cpuMesh : cpuData.meshes) {
    ModelBinaryLoader::MeshBinaryData meshBinaryData;
    meshBinaryData.vertices = cpuMesh.vertices;
    meshBinaryData.indices = cpuMesh.indices;
    meshBinaryData.material = materialGuids[cpuMesh.materialIndex];
    modelBinaryData.meshes.push_back(meshBinaryData);
  }
  auto modelPath =
      mAssetManager.GetAssetsRoot() /
      (relPath.string() + FileTypeUtils::ToPath(FileType::ModelCache));
  ModelBinaryLoader::ToBinaryFile(modelBinaryData, modelPath);

  AssetMeta meta;
  meta.handle = AssetHandle(AssetType::MODEL, GUID(), relPath);
  meta.importSettings = ModelLoader::ImportSettings();
  meta.dependencies = materialGuids;

  writeMeta(relPath, meta);
  mAssetManager.RegisterAssetMeta(meta);

  return meta.handle;
}

void AssetImporter::writeMeta(const std::filesystem::path &relPath,
                              const AssetMeta &meta) {
  YAML::Node node(meta);
  std::filesystem::path outputPath =
      mAssetManager.GetAssetsRoot() /
      (relPath.string() + FileTypeUtils::ToPath(FileType::AssetMeta));
  std::ofstream(outputPath) << node;
}
} // namespace Ember
