#include "AssetManager.h"

#include "AssetMeta.h"
#include "CPU/Loaders/MaterialLoader.h"
#include "CPU/Loaders/TextureLoader.h"
#include "GPU/Loaders/GPUMesh.h"
#include "GPU/Loaders/GPUTexture.h"
#include "Project/Assets/Assets.h"
#include "Project/Assets/CPU/Loaders/ModelBinaryLoader.h"
#include "Project/Assets/CPU/Loaders/ModelLoader.h"
#include "Project/FileTypes.h"

#define ENGINE_TEXTURES "../../Ember/res/defaults/"

namespace Ember {
AssetManager::AssetManager(const std::filesystem::path &assetRoot)
    : mAssetsRoot(assetRoot) {}

std::optional<AssetHandle> AssetManager::LoadMaterial(const AssetMeta &meta) {
  if (mAssetPool.HasAsset(meta.handle.guid)) return meta.handle;
  // Load texture dependencies
  for (const GUID &texGuid : meta.dependencies) {
    if (texGuid == GUID::NONE()) continue;
    if (mAssetPool.HasAsset(texGuid)) continue;
    auto metaOpt = mDatabase.GetAssetMetaByGuid(texGuid);
    if (metaOpt) {
      LoadTexture(*metaOpt);
    }
  }

  // Load material
  auto matOpt = MaterialLoader::FromFile(mAssetsRoot / meta.handle.path);
  if (!matOpt) return std::nullopt;
  mAssetPool.StoreAsset(meta.handle,
                        std::make_shared<Asset::Material>(*matOpt));
  return meta.handle;
}

std::optional<AssetHandle> AssetManager::LoadTexture(const AssetMeta &meta) {
  if (mAssetPool.HasAsset(meta.handle.guid)) return meta.handle;

  auto texOpt = TextureLoader::FromFile(mAssetsRoot / meta.handle.path);
  if (!texOpt) return std::nullopt;

  auto gpuTex = GPUTexture::FromTextureData(
      *texOpt, std::get<TextureLoader::ImportSettings>(meta.importSettings));

  Asset::Texture tex;
  tex.gpu = gpuTex;
  mAssetPool.StoreAsset(meta.handle, std::make_shared<Asset::Texture>(tex));
  return meta.handle;
}

std::optional<AssetHandle> AssetManager::LoadModel(const AssetMeta &meta) {
  if (mAssetPool.HasAsset(meta.handle.guid)) return meta.handle;

  // Load material dependencies
  for (const GUID &matGuid : meta.dependencies) {
    if (mAssetPool.HasAsset(matGuid)) continue;
    auto metaOpt = mDatabase.GetAssetMetaByGuid(matGuid);
    if (metaOpt) {
      LoadMaterial(*metaOpt);
    }
  }

  // Load model
  auto binaryFile = mAssetsRoot / (meta.handle.path.string() +
                                   FileTypeUtils::ToPath(FileType::ModelCache));
  auto modelBinaryDataOpt = ModelBinaryLoader::FromBinaryFile(binaryFile);
  if (!modelBinaryDataOpt) return std::nullopt;

  Asset::Model runtimeModel;
  for (size_t i = 0; i < modelBinaryDataOpt->meshes.size(); i++) {
    const ModelBinaryLoader::MeshBinaryData &meshBinaryData =
        modelBinaryDataOpt->meshes[i];

    Asset::Mesh runtimeMesh;
    runtimeMesh.gpu =
        GPUMesh::UploadMesh(meshBinaryData.vertices, meshBinaryData.indices);
    runtimeMesh.material = meshBinaryData.material;
    runtimeModel.meshes.push_back(runtimeMesh);
  }

  mAssetPool.StoreAsset(meta.handle,
                        std::make_shared<Asset::Model>(runtimeModel));

  return meta.handle;
}

void AssetManager::RegisterAssetMeta(const AssetMeta &assetMeta) {
  mDatabase.RegisterAssetMeta(assetMeta);
}
std::optional<AssetMeta>
AssetManager::GetAssetMetaByGuid(const GUID &guid) const {
  return mDatabase.GetAssetMetaByGuid(guid);
}
std::optional<AssetMeta>
AssetManager::GetAssetMetaByPath(const std::filesystem::path &path) const {
  return mDatabase.GetAssetMetaByPath(path);
}
std::vector<AssetHandle> AssetManager::GetAllAssetHandles() const {
  return mDatabase.GetAllAssetHandles();
}
std::vector<AssetHandle>
AssetManager::GetAssetHandlesByType(const AssetType &type) const {
  return mDatabase.GetAssetHandlesByType(type);
}

void AssetManager::RemoveAsset(const GUID &guid) {
  mAssetPool.RemoveAsset(guid);
  mDatabase.RemoveAssetMeta(guid);
}
} // namespace Ember
