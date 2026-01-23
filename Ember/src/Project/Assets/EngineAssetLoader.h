#pragma once

#include "AssetManager.h"

namespace Ember {

namespace BuiltInAssets {
const GUID PrimitiveCube = 1;
const GUID PrimitiveSphere = 2;
const GUID PrimitiveCylinder = 3;
const GUID DefaultMaterial = 10;
} // namespace BuiltInAssets

class EngineAssetLoader {
public:
  static void LoadBuiltInAssets(AssetManager &assetManager);

  static const AssetHandle &GetCubeHandle() { return sPrimitiveCubeHandle; }
  static const AssetHandle &GetSphereHandle() { return sPrimitiveSphereHandle; }
  static const AssetHandle &GetCylinderHandle() {
    return sPrimitiveCylinderHandle;
  }
  static const AssetHandle &GetDefaultMaterialHandle() {
    return sDefaultMaterialHandle;
  }

private:
  // TODO this is duplicate from assetmanager LoadModel, fix
  static Asset::Model generateModel(const CPUAsset::ModelData& modelData);

private:
  static AssetHandle sPrimitiveCubeHandle;
  static AssetHandle sPrimitiveSphereHandle;
  static AssetHandle sPrimitiveCylinderHandle;

  static AssetHandle sDefaultMaterialHandle;
};
} // namespace Ember
