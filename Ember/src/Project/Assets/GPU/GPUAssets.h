#pragma once

#include <memory>

#include "Project/Assets/CPU/CPUAssets.h"
#include "Renderer/Buffers/EBO.h"
#include "Renderer/Buffers/VAO.h"
#include "Renderer/Buffers/VBO.h"

namespace Ember {
namespace GPUAsset {
struct GPUTexture {
  GLuint id = 0;
};
struct GPUMesh {
  std::shared_ptr<VAO> vao = nullptr;
  std::shared_ptr<VBO<CPUAsset::MeshData::Vertex>> vbo = nullptr;
  std::shared_ptr<EBO> ebo = nullptr;
  int indexCount = 0;
};
struct GPUModel {
  std::vector<GPUMesh> meshes;
};
} // namespace GPUAsset
} // namespace Ember
