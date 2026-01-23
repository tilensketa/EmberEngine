#include "ModelLoader.h"
#include "Core/Logger.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace Ember::ModelLoader {

CPUAsset::ModelData CreateCube() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;

  const float h = 0.5f;

  mesh.vertices = {
      // +X face
      {{h, -h, -h}, {1, 0, 0}, {0, 0}, {0, 0, -1}},
      {{h, h, -h}, {1, 0, 0}, {1, 0}, {0, 0, -1}},
      {{h, h, h}, {1, 0, 0}, {1, 1}, {0, 0, -1}},
      {{h, -h, h}, {1, 0, 0}, {0, 1}, {0, 0, -1}},

      // -X face
      {{-h, -h, h}, {-1, 0, 0}, {0, 0}, {0, 0, 1}},
      {{-h, h, h}, {-1, 0, 0}, {1, 0}, {0, 0, 1}},
      {{-h, h, -h}, {-1, 0, 0}, {1, 1}, {0, 0, 1}},
      {{-h, -h, -h}, {-1, 0, 0}, {0, 1}, {0, 0, 1}},

      // +Y face
      {{-h, h, -h}, {0, 1, 0}, {0, 0}, {1, 0, 0}},
      {{-h, h, h}, {0, 1, 0}, {0, 1}, {1, 0, 0}},
      {{h, h, h}, {0, 1, 0}, {1, 1}, {1, 0, 0}},
      {{h, h, -h}, {0, 1, 0}, {1, 0}, {1, 0, 0}},

      // -Y face
      {{-h, -h, h}, {0, -1, 0}, {0, 0}, {1, 0, 0}},
      {{-h, -h, -h}, {0, -1, 0}, {0, 1}, {1, 0, 0}},
      {{h, -h, -h}, {0, -1, 0}, {1, 1}, {1, 0, 0}},
      {{h, -h, h}, {0, -1, 0}, {1, 0}, {1, 0, 0}},

      // +Z face
      {{-h, -h, h}, {0, 0, 1}, {0, 0}, {1, 0, 0}},
      {{h, -h, h}, {0, 0, 1}, {1, 0}, {1, 0, 0}},
      {{h, h, h}, {0, 0, 1}, {1, 1}, {1, 0, 0}},
      {{-h, h, h}, {0, 0, 1}, {0, 1}, {1, 0, 0}},

      // -Z face
      {{h, -h, -h}, {0, 0, -1}, {0, 0}, {-1, 0, 0}},
      {{-h, -h, -h}, {0, 0, -1}, {1, 0}, {-1, 0, 0}},
      {{-h, h, -h}, {0, 0, -1}, {1, 1}, {-1, 0, 0}},
      {{h, h, -h}, {0, 0, -1}, {0, 1}, {-1, 0, 0}},
  };

  mesh.indices = {
      0,  1,  2,  0,  2,  3,  // +X
      4,  5,  6,  4,  6,  7,  // -X
      8,  9,  10, 8,  10, 11, // +Y
      12, 13, 14, 12, 14, 15, // -Y
      16, 17, 18, 16, 18, 19, // +Z
      20, 21, 22, 20, 22, 23  // -Z
  };

  CPUAsset::ModelData modelData;
  modelData.meshes.push_back(mesh);
  return modelData;
}

CPUAsset::ModelData CreateSphere() {
  uint32_t slices = 32;
  uint32_t stacks = 16;
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;

  const float radius = 0.5f;

  for (uint32_t y = 0; y <= stacks; y++) {
    float v = (float)y / stacks;
    float phi = v * glm::pi<float>();

    for (uint32_t x = 0; x <= slices; x++) {
      float u = (float)x / slices;
      float theta = u * glm::two_pi<float>();

      glm::vec3 pos{radius * std::sin(phi) * std::cos(theta),
                    radius * std::cos(phi),
                    radius * std::sin(phi) * std::sin(theta)};

      glm::vec3 normal = glm::normalize(pos);

      mesh.vertices.push_back({pos, normal, {u, 1.0f - v}, glm::vec3(1, 0, 0)});
    }
  }

  uint32_t stride = slices + 1;
  for (uint32_t y = 0; y < stacks; y++) {
    for (uint32_t x = 0; x < slices; x++) {
      uint32_t i0 = y * stride + x;
      uint32_t i1 = i0 + stride;

      mesh.indices.insert(mesh.indices.end(),
                          {i0, i1, i0 + 1, i0 + 1, i1, i1 + 1});
    }
  }

  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreateCylinder() {
  uint32_t slices = 32;
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;

  const float radius = 0.5f;
  const float height = 1.0f;
  const float halfH = height * 0.5f;

  // --- Body ---
  for (uint32_t i = 0; i <= slices; i++) {
    float u = (float)i / slices;
    float theta = u * glm::two_pi<float>();

    float x = std::cos(theta);
    float z = std::sin(theta);

    glm::vec3 normal = {x, 0, z};

    mesh.vertices.push_back(
        {{radius * x, -halfH, radius * z}, normal, {u, 0}, glm::vec3(1, 0, 0)});

    mesh.vertices.push_back(
        {{radius * x, halfH, radius * z}, normal, {u, 1}, glm::vec3(1, 0, 0)});
  }

  for (uint32_t i = 0; i < slices; i++) {
    uint32_t base = i * 2;
    mesh.indices.insert(mesh.indices.end(), {base, base + 1, base + 2, base + 2,
                                             base + 1, base + 3});
  }

  // --- Caps ---
  auto addCap = [&](float y, float normalY) {
    uint32_t centerIndex = mesh.vertices.size();
    mesh.vertices.push_back(
        {{0, y, 0}, {0, normalY, 0}, {0.5f, 0.5f}, glm::vec3(1, 0, 0)});

    for (uint32_t i = 0; i <= slices; i++) {
      float u = (float)i / slices;
      float theta = u * glm::two_pi<float>();

      float x = std::cos(theta);
      float z = std::sin(theta);

      mesh.vertices.push_back({{radius * x, y, radius * z},
                               {0, normalY, 0},
                               {x * 0.5f + 0.5f, z * 0.5f + 0.5f},
                               glm::vec3(1, 0, 0)});
    }

    for (uint32_t i = 0; i < slices; i++) {
      if (normalY > 0)
        mesh.indices.insert(
            mesh.indices.end(),
            {centerIndex, centerIndex + i + 1, centerIndex + i + 2});
      else
        mesh.indices.insert(
            mesh.indices.end(),
            {centerIndex, centerIndex + i + 2, centerIndex + i + 1});
    }
  };

  addCap(halfH, 1.0f);
  addCap(-halfH, -1.0f);

  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

std::vector<std::filesystem::path>
loadMaterialTexturePaths(const aiMaterial *aiMat, aiTextureType type) {
  std::vector<std::filesystem::path> textures;

  for (size_t i = 0; i < aiMat->GetTextureCount(type); i++) {
    aiString str;
    aiMat->GetTexture(type, i, &str);
    std::filesystem::path texPath(str.C_Str());
    textures.push_back(texPath);
  }
  return textures;
}

CPUAsset::MeshData processMesh(aiMesh *mesh, const aiScene *scene) {
  CPUAsset::MeshData cpuMesh;
  // vertices
  for (size_t i = 0; i < mesh->mNumVertices; i++) {
    CPUAsset::MeshData::Vertex vertex;
    const auto &v = mesh->mVertices[i];
    const auto &n = mesh->mNormals[i];
    vertex.position = glm::vec3(v.x, v.y, v.z);
    vertex.normal = glm::vec3(n.x, n.y, n.z);

    if (mesh->HasTextureCoords(0)) {
      const auto &t = mesh->mTextureCoords[0][i];
      vertex.texCoord = glm::vec2(t.x, t.y);
    }
    if (mesh->HasTangentsAndBitangents()) {
      const auto &t = mesh->mTangents[i];
      vertex.tangent = glm::vec3(t.x, t.y, t.z);
    }
    cpuMesh.vertices.push_back(vertex);
  }
  // Indices
  for (size_t i = 0; i < mesh->mNumFaces; i++) {
    const auto &f = mesh->mFaces[i];
    cpuMesh.indices.insert(cpuMesh.indices.end(), f.mIndices,
                           f.mIndices + f.mNumIndices);
  }
  // Material
  cpuMesh.materialIndex = mesh->mMaterialIndex;

  return cpuMesh;
}

void processNode(aiNode *node, const aiScene *scene, CPUAsset::ModelData &model,
                 const glm::mat4 &parentTransform = glm::mat4(1.0f)) {

  aiMatrix4x4 m = node->mTransformation;
  glm::mat4 nodeTransform =
      glm::mat4(m.a1, m.b1, m.c1, m.d1, m.a2, m.b2, m.c2, m.d2, m.a3, m.b3,
                m.c3, m.d3, m.a4, m.b4, m.c4, m.d4);
  glm::mat4 globalTransform = parentTransform * nodeTransform;

  for (size_t i = 0; i < node->mNumMeshes; i++) {
    aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
    CPUAsset::MeshData meshData = processMesh(mesh, scene);

    for (auto &v : meshData.vertices) {
      glm::vec4 pos = globalTransform * glm::vec4(v.position, 1.0f);
      v.position = glm::vec3(pos) / pos.w; // perspective divide just in case
      glm::vec4 normal = glm::transpose(glm::inverse(globalTransform)) *
                         glm::vec4(v.normal, 0.0f);
      v.normal = glm::normalize(glm::vec3(normal));
    }

    model.meshes.push_back(meshData);
  }

  for (size_t i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene, model, globalTransform);
  }
}

void processMaterials(const aiScene *scene, CPUAsset::ModelData &model,
                      const std::filesystem::path &modelPath) {
  for (size_t i = 0; i < scene->mNumMaterials; i++) {
    const auto &aiMat = scene->mMaterials[i];
    CPUAsset::MaterialData matData;

    auto processTexture = [&](std::filesystem::path &path,
                              const aiMaterial *aiMat, aiTextureType type) {
      auto textures = loadMaterialTexturePaths(aiMat, type);
      if (!textures.empty()) {
        path = modelPath.parent_path() / textures[0];
        path = path.lexically_normal();
      }
    };
    processTexture(matData.albedoMap, aiMat, aiTextureType_DIFFUSE);
    processTexture(matData.normalMap, aiMat, aiTextureType_NORMALS);
    processTexture(matData.metalnessMap, aiMat, aiTextureType_METALNESS);
    processTexture(matData.roughnessMap, aiMat,
                   aiTextureType_DIFFUSE_ROUGHNESS);

    CPUAsset::MaterialFactors matFact;
    aiColor3D d;
    aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, d);
    matFact.albedoColor = glm::vec3(d.r, d.g, d.b);
    aiMat->Get(AI_MATKEY_EMISSIVE_INTENSITY, matFact.emission);
    aiMat->Get(AI_MATKEY_METALLIC_FACTOR, matFact.metalness);
    aiMat->Get(AI_MATKEY_ROUGHNESS_FACTOR, matFact.roughness);
    matData.matFactors = matFact;

    model.materials.push_back(matData);
  }
}

std::optional<CPUAsset::ModelData> FromFile(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) return std::nullopt;

  Assimp::Importer importer;
  const aiScene *scene = importer.ReadFile(
      path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
                aiProcess_GlobalScale | aiProcess_CalcTangentSpace);

  if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ||
      !scene->mRootNode) {
    LOG_ERROR("ModelLoader::FromFile: Assimp error loading from path: {}",
              path.string());
    return std::nullopt;
  }

  CPUAsset::ModelData model;

  glm::mat4 parent(1.0f);
  parent = glm::rotate(parent, glm::radians(90.0f), glm::vec3(1, 0, 0));
  processNode(scene->mRootNode, scene, model, parent);
  processMaterials(scene, model, path);

  return model;
}

} // namespace Ember::ModelLoader
