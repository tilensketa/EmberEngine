#include "EngineAssetLoader.h"

#include "Project/Assets/CPU/Loaders/ModelBinaryLoader.h"
#include "Project/Assets/CPU/Loaders/ModelLoader.h"
#include "Project/Assets/GPU/Loaders/GPUMesh.h"

namespace Ember {

AssetHandle EngineAssetLoader::sPrimitiveCubeHandle;
AssetHandle EngineAssetLoader::sPrimitiveSphereHandle;
AssetHandle EngineAssetLoader::sPrimitiveCylinderHandle;

AssetHandle EngineAssetLoader::sDefaultMaterialHandle;

// TODO don't modify asset pool
void EngineAssetLoader::LoadBuiltInAssets(AssetManager &assetManager) {
  { // Default Material
    sDefaultMaterialHandle = AssetHandle(
        AssetType::MATERIAL, BuiltInAssets::DefaultMaterial, "DefaultMaterial");
    Asset::Material mat;
    AssetMeta assetMeta;
    assetMeta.handle = sDefaultMaterialHandle;
    assetMeta.importSettings = MaterialLoader::ImportSettings();
    assetManager.RegisterAssetMeta(assetMeta);
    assetManager.GetAssetPool().StoreAsset(
        sDefaultMaterialHandle, std::make_shared<Asset::Material>(mat));
  }
  { // Primitive Cube
    CPUAsset::ModelData cpuData = ModelLoader::CreateCube();
    Asset::Model model = generateModel(cpuData);

    sPrimitiveCubeHandle = AssetHandle(
        AssetType::MODEL, BuiltInAssets::PrimitiveCube, "PrimitiveCube");
    AssetMeta assetMeta(sPrimitiveCubeHandle, ModelLoader::ImportSettings());

    assetManager.RegisterAssetMeta(assetMeta);
    assetManager.GetAssetPool().StoreAsset(
        sPrimitiveCubeHandle, std::make_shared<Asset::Model>(model));
  }
  { // Primitive Sphere
    CPUAsset::ModelData cpuData = ModelLoader::CreateSphere();
    Asset::Model model = generateModel(cpuData);

    sPrimitiveSphereHandle = AssetHandle(
        AssetType::MODEL, BuiltInAssets::PrimitiveSphere, "PrimitiveSphere");
    AssetMeta assetMeta(sPrimitiveSphereHandle, ModelLoader::ImportSettings());

    assetManager.RegisterAssetMeta(assetMeta);
    assetManager.GetAssetPool().StoreAsset(
        sPrimitiveSphereHandle, std::make_shared<Asset::Model>(model));
  }
  { // Primitive Cylinder
    CPUAsset::ModelData cpuData = ModelLoader::CreateCylinder();
    Asset::Model model = generateModel(cpuData);

    sPrimitiveCylinderHandle =
        AssetHandle(AssetType::MODEL, BuiltInAssets::PrimitiveCylinder,
                    "PrimitiveCylinder");
    AssetMeta assetMeta(sPrimitiveCylinderHandle, ModelLoader::ImportSettings());

    assetManager.RegisterAssetMeta(assetMeta);
    assetManager.GetAssetPool().StoreAsset(
        sPrimitiveCylinderHandle, std::make_shared<Asset::Model>(model));
  }
}

Asset::Model
EngineAssetLoader::generateModel(const CPUAsset::ModelData &modelData) {
  ModelBinaryLoader::ModelBinaryData modelBinaryData;
  for (size_t i = 0; i < modelData.meshes.size(); i++) {
    ModelBinaryLoader::MeshBinaryData meshBinaryData;
    meshBinaryData.material = sDefaultMaterialHandle.guid;
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
