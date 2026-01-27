#pragma once

#include "AssetManager.h"

namespace Ember {
enum class EnginePrimitive : uint8_t {
  Plane,
  Cube,
  Circle,
  Sphere,
  Cylinder,
  Capsule,
  Cone,
  Pyramid
};
enum class EngineMaterial : uint8_t { DefaultMaterial };

inline GUID EnginePrimitiveGuid(EnginePrimitive primitive) {
  return GUID(1 + static_cast<uint64_t>(primitive));
}
inline GUID EngineMaterialGuid(EngineMaterial material) {
  return GUID(10 + static_cast<uint64_t>(material));
}
struct EnginePrimitiveDesc {
  EnginePrimitive primitive;
  const char *name;
  CPUAsset::ModelData modelData;
};
static EnginePrimitiveDesc sEnginePrimitives[] = {
    {EnginePrimitive::Plane, "Plane", ModelLoader::CreatePlane()},
    {EnginePrimitive::Cube, "Cube", ModelLoader::CreateCube()},
    {EnginePrimitive::Sphere, "Sphere", ModelLoader::CreateSphere()},
    {EnginePrimitive::Circle, "Circle", ModelLoader::CreateCircle()},
    {EnginePrimitive::Cylinder, "Cylinder", ModelLoader::CreateCylinder()},
    {EnginePrimitive::Capsule, "Capsule", ModelLoader::CreateCapsule()},
    {EnginePrimitive::Cone, "Cone", ModelLoader::CreateCone()},
    {EnginePrimitive::Pyramid, "Pyramid", ModelLoader::CreatePyramid()},
};

struct EngineMaterialDesc {
  EngineMaterial material;
  const char *name;
};
static constexpr EngineMaterialDesc sEngineMaterials[] = {
    {EngineMaterial::DefaultMaterial, "DefaultMaterial"}};

class EngineAssetLoader {
public:
  static void LoadBuiltInAssets(AssetManager &assetManager);

  static const AssetHandle &GetPrimitive(EnginePrimitive primitive) {
    return sPrimitives.at(primitive);
  }
  static const AssetHandle &GetMaterial(EngineMaterial material) {
    return sMaterials.at(material);
  }

private:
  // TODO this is duplicate from assetmanager LoadModel, fix
  static Asset::Model generateModel(const CPUAsset::ModelData &modelData);

private:
  static std::unordered_map<EnginePrimitive, AssetHandle> sPrimitives;
  static std::unordered_map<EngineMaterial, AssetHandle> sMaterials;
};
} // namespace Ember
