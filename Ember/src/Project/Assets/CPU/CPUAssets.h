#pragma once

#include <cstdint>
#include <filesystem>
#include <glm/glm.hpp>
#include <vector>

#include <glad/glad.h>

namespace Ember::CPUAsset {
struct TextureData {
  std::vector<uint8_t> pixels;
  uint32_t width, height, channels;
};
struct MaterialFactors {
  glm::vec3 albedoColor = glm::vec3(1.0f);
  float emission = 0.0f;
  float metalness = 0.0f;
  float roughness = 1.0f;
};
struct MaterialData {
  MaterialFactors matFactors;

  // Absolute paths
  std::filesystem::path albedoMap;
  std::filesystem::path normalMap;
  std::filesystem::path metalnessMap;
  std::filesystem::path roughnessMap;
};
struct MeshData {
  struct Vertex {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec2 texCoord = glm::vec2(0.0f);
    glm::vec3 tangent = glm::vec3(1.0f);
  };
  std::vector<Vertex> vertices;
  std::vector<GLuint> indices;
  uint64_t materialIndex = 0;
};
struct ModelData {
  std::vector<MeshData> meshes;
  std::vector<MaterialData> materials;
};
} // namespace Ember::CPUAsset
