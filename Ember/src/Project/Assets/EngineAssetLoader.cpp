#include "EngineAssetLoader.h"

#include "Project/Assets/CPU/Loaders/ModelBinaryLoader.h"
#include "Project/Assets/CPU/Loaders/ModelLoader.h"
#include "Project/Assets/GPU/Loaders/GPUMesh.h"

namespace Ember {

std::unordered_map<EnginePrimitive, AssetHandle> EngineAssetLoader::sPrimitives;
std::unordered_map<EngineMaterial, AssetHandle> EngineAssetLoader::sMaterials;

void EngineAssetLoader::LoadBuiltInAssets(AssetManager &assetManager) {
  for (const auto &desc : sEngineMaterials) {
    const GUID guid = EngineMaterialGuid(desc.material);
    AssetHandle handle(AssetType::MATERIAL, guid, desc.name);

    AssetMeta assetMeta(handle, MaterialLoader::ImportSettings());
    assetManager.RegisterAssetMeta(assetMeta);
    assetManager.GetAssetPool().StoreAsset(
        handle, std::make_shared<Asset::Material>(Asset::Material()));
    sMaterials.emplace(desc.material, handle);
  }
  for (const auto &desc : sEnginePrimitives) {
    const GUID guid = EnginePrimitiveGuid(desc.primitive);
    AssetHandle handle(AssetType::MODEL, guid, desc.name);

    auto model = generateModel(desc.modelData);

    AssetMeta assetMeta(handle, ModelLoader::ImportSettings());
    assetManager.RegisterAssetMeta(assetMeta);
    assetManager.GetAssetPool().StoreAsset(
        handle, std::make_shared<Asset::Model>(model));
    sPrimitives.emplace(desc.primitive, handle);
  }
}

Asset::Model
EngineAssetLoader::generateModel(const CPUAsset::ModelData &modelData) {
  ModelBinaryLoader::ModelBinaryData modelBinaryData;
  for (size_t i = 0; i < modelData.meshes.size(); i++) {
    ModelBinaryLoader::MeshBinaryData meshBinaryData;
    meshBinaryData.material = EngineMaterialGuid(EngineMaterial::DefaultMaterial);
    meshBinaryData.vertices = modelData.meshes[i].vertices;
    meshBinaryData.indices = modelData.meshes[i].indices;
    modelBinaryData.meshes.push_back(meshBinaryData);
  }

  Asset::Model runtimeModel;
  for (size_t i = 0; i < modelBinaryData.meshes.size(); i++) {
    const ModelBinaryLoader::MeshBinaryData &meshBinaryData =
        modelBinaryData.meshes[i];

    Asset::Mesh runtimeMesh;
    runtimeMesh.gpu =
        GPUMesh::UploadMesh(meshBinaryData.vertices, meshBinaryData.indices);
    runtimeMesh.material = meshBinaryData.material;
    runtimeModel.meshes.push_back(runtimeMesh);
  }
  return runtimeModel;
}
} // namespace Ember
