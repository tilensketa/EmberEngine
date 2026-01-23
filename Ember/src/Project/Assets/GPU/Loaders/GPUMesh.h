#pragma once

#include "../GPUAssets.h"

namespace Ember {
namespace GPUMesh {
inline GPUAsset::GPUMesh
UploadMesh(const std::vector<CPUAsset::MeshData::Vertex> &vertices,
           const std::vector<GLuint> &indices) {
  std::shared_ptr<VAO> Vao = std::make_shared<VAO>();
  Vao->Bind();
  std::shared_ptr<VBO<CPUAsset::MeshData::Vertex>> Vbo =
      std::make_shared<VBO<CPUAsset::MeshData::Vertex>>(vertices);
  std::shared_ptr<EBO> Ebo = std::make_shared<EBO>(indices);

  constexpr GLsizeiptr size = sizeof(CPUAsset::MeshData::Vertex);
  Vao->LinkAttrib(*Vbo, 0, 3, GL_FLOAT, size, (void *)0);
  Vao->LinkAttrib(*Vbo, 1, 3, GL_FLOAT, size, (void *)(3 * sizeof(float)));
  Vao->LinkAttrib(*Vbo, 2, 2, GL_FLOAT, size, (void *)(6 * sizeof(float)));
  Vao->LinkAttrib(*Vbo, 3, 3, GL_FLOAT, size, (void *)(8 * sizeof(float)));
  Vao->Unbind();
  int indexCount = static_cast<int>(indices.size());

  return GPUAsset::GPUMesh{Vao, Vbo, Ebo, indexCount};
}
inline GPUAsset::GPUMesh FromMeshData(const CPUAsset::MeshData &meshData) {
  return UploadMesh(meshData.vertices, meshData.indices);
}
} // namespace GPUMesh
} // namespace Ember
