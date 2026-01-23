#pragma once

#include "Core/GUID.h"
#include "Project/Assets/CPU/CPUAssets.h"

namespace Ember {
namespace ModelBinaryLoader {
struct MeshBinaryData {
  std::vector<CPUAsset::MeshData::Vertex> vertices;
  std::vector<GLuint> indices;
  GUID material;
};
struct ModelBinaryData {
  std::vector<MeshBinaryData> meshes;
};

std::optional<ModelBinaryData> FromBinaryFile(const std::filesystem::path &path);
void ToBinaryFile(const ModelBinaryData &modelBinaryData,
                  const std::filesystem::path &path);

} // namespace ModelBinaryLoader
} // namespace Ember
