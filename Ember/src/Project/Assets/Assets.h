#pragma once

#include "Core/GUID.h"

#include "CPU/CPUAssets.h"
#include "GPU/GPUAssets.h"

namespace Ember::Asset {

struct Texture {
  GPUAsset::GPUTexture gpu;
};

struct Material {
  CPUAsset::MaterialFactors factors;
  GUID albedoMap = GUID::NONE();
  GUID normalMap = GUID::NONE();
  GUID metalnessMap = GUID::NONE();
  GUID roughnessMap = GUID::NONE();
};

struct Mesh {
  GPUAsset::GPUMesh gpu;
  GUID material = GUID::NONE();
};

struct Model {
  std::vector<Mesh> meshes;
};

} // namespace Ember::Asset
