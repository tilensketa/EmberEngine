#include "ModelLoader.h"
#include "Core/Logger.h"

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#define RESOLUTION 16

namespace Ember::ModelLoader {

// ---------------------- Helpers ----------------------
static void buildBasis(const glm::vec3 &n, glm::vec3 &t, glm::vec3 &b) {
  glm::vec3 up =
      (std::abs(n.z) < 0.999f) ? glm::vec3(0, 0, 1) : glm::vec3(0, 1, 0);
  t = glm::normalize(glm::cross(up, n));
  b = glm::normalize(glm::cross(n, t));
}

void addQuad(CPUAsset::MeshData &mesh, float size, const glm::vec3 &pos,
             const glm::vec3 &normal) {
  glm::vec3 n = glm::normalize(normal);
  glm::vec3 t, b;
  buildBasis(n, t, b);
  float h = size * 0.5f;
  uint32_t base = mesh.vertices.size();

  mesh.vertices.push_back({pos + (-t - b) * h, n, {0, 0}, t});
  mesh.vertices.push_back({pos + (t - b) * h, n, {1, 0}, t});
  mesh.vertices.push_back({pos + (t + b) * h, n, {1, 1}, t});
  mesh.vertices.push_back({pos + (-t + b) * h, n, {0, 1}, t});

  mesh.indices.insert(mesh.indices.end(),
                      {base, base + 1, base + 2, base, base + 2, base + 3});
}

void addDisk(CPUAsset::MeshData &mesh, float radius, const glm::vec3 &pos,
             const glm::vec3 &normal, uint32_t slices = 32) {
  glm::vec3 n = glm::normalize(normal);
  glm::vec3 t, b;
  buildBasis(n, t, b);

  uint32_t centerIndex = mesh.vertices.size();
  mesh.vertices.push_back({pos, n, {0.5f, 0.5f}, t});

  for (uint32_t i = 0; i <= slices; i++) {
    float theta = float(i) / slices * glm::two_pi<float>();
    float cx = std::cos(theta);
    float cy = std::sin(theta);
    glm::vec3 offset = (t * cx + b * cy) * radius;

    mesh.vertices.push_back(
        {pos + offset, n, {cx * 0.5f + 0.5f, cy * 0.5f + 0.5f}, t});
  }

  for (uint32_t i = 0; i < slices; i++) {
    mesh.indices.insert(mesh.indices.end(), {centerIndex, centerIndex + i + 1,
                                             centerIndex + i + 2});
  }
}

void addCylinderSide(CPUAsset::MeshData &mesh, float radius, float height,
                     uint32_t slices = 32) {
  float halfH = height * 0.5f;
  uint32_t baseIndex = mesh.vertices.size();

  for (uint32_t i = 0; i <= slices; i++) {
    float theta = float(i) / slices * glm::two_pi<float>();
    float x = std::cos(theta);
    float y = std::sin(theta);
    glm::vec3 normal{x, y, 0};
    glm::vec3 tangent{-y, x, 0};

    mesh.vertices.push_back({{radius * x, radius * y, -halfH},
                             normal,
                             {float(i) / slices, 0},
                             tangent});
    mesh.vertices.push_back({{radius * x, radius * y, halfH},
                             normal,
                             {float(i) / slices, 1},
                             tangent});
  }

  for (uint32_t i = 0; i < slices; i++) {
    uint32_t b = baseIndex + i * 2;
    mesh.indices.insert(mesh.indices.end(),
                        {b, b + 1, b + 2, b + 2, b + 1, b + 3});
  }
}

void addHemisphere(CPUAsset::MeshData &mesh, float radius, float centerZ,
                   float sign = 1.0f, uint32_t slices = 32,
                   uint32_t stacks = 16) {
  uint32_t base = mesh.vertices.size();

  for (uint32_t y = 0; y <= stacks; y++) {
    float v = float(y) / stacks;
    float phi = v * glm::half_pi<float>();
    float r = std::cos(phi) * radius;
    float z = centerZ + sign * std::sin(phi) * radius;

    for (uint32_t i = 0; i <= slices; i++) {
      float theta = float(i) / slices * glm::two_pi<float>();
      float x = std::cos(theta);
      float y = std::sin(theta);

      glm::vec3 normal = glm::normalize(glm::vec3(x, y, sign * std::sin(phi)));
      glm::vec3 tangent{-y, x, 0};

      mesh.vertices.push_back(
          {{r * x, r * y, z}, normal, {float(i) / slices, v}, tangent});
    }

    if (y > 0) {
      uint32_t prev = base + (y - 1) * (slices + 1);
      uint32_t curr = base + y * (slices + 1);
      for (uint32_t i = 0; i < slices; i++) {
        mesh.indices.insert(mesh.indices.end(),
                            {prev + i, curr + i, prev + i + 1, prev + i + 1,
                             curr + i, curr + i + 1});
      }
    }
  }
}

// ---------------------- Primitives ----------------------

CPUAsset::ModelData CreatePlane() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  addQuad(mesh, 1.0f, {0, 0, 0}, {0, 0, 1});
  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreateCircle() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  addDisk(mesh, 0.5f, {0, 0, 0}, {0, 0, 1});
  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreateCube() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  const float h = 0.5f;
  // +X, -X, +Y, -Y, +Z, -Z faces
  addQuad(mesh, 1.0f, {h, 0, 0}, {1, 0, 0});
  addQuad(mesh, 1.0f, {-h, 0, 0}, {-1, 0, 0});
  addQuad(mesh, 1.0f, {0, h, 0}, {0, 1, 0});
  addQuad(mesh, 1.0f, {0, -h, 0}, {0, -1, 0});
  addQuad(mesh, 1.0f, {0, 0, h}, {0, 0, 1});
  addQuad(mesh, 1.0f, {0, 0, -h}, {0, 0, -1});
  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}
CPUAsset::ModelData CreateSphere() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  const float radius = 0.5f;
  const uint32_t slices = 32;
  const uint32_t stacks = 16;

  addHemisphere(mesh, radius, 0.0f, 1.0f, slices, stacks);
  addHemisphere(mesh, radius, 0.0f, -1.0f, slices, stacks);

  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreateCylinder() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  float radius = 0.5f, height = 1.0f;
  addCylinderSide(mesh, radius, height);
  addDisk(mesh, radius, {0, 0, height / 2}, {0, 0, 1});
  addDisk(mesh, radius, {0, 0, -height / 2}, {0, 0, -1});
  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreateCapsule() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  float radius = 0.5f, height = 1.0f;
  addCylinderSide(mesh, radius, height);
  addHemisphere(mesh, radius, height / 2, 1);
  addHemisphere(mesh, radius, -height / 2, -1);
  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreateCone() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  const uint32_t slices = 32;
  float radius = 0.5f, height = 1.0f;
  uint32_t tipIndex = mesh.vertices.size();
  mesh.vertices.push_back(
      {{0, 0, height / 2}, {0, 0, 1}, {0.5f, 1.0f}, {1, 0, 0}});

  uint32_t baseStart = mesh.vertices.size();
  for (uint32_t i = 0; i <= slices; i++) {
    float theta = float(i) / slices * glm::two_pi<float>();
    float x = std::cos(theta), y = std::sin(theta);
    glm::vec3 normal = glm::normalize(glm::vec3(x, y, radius / height));
    glm::vec3 tangent = {-y, x, 0};
    mesh.vertices.push_back({{radius * x, radius * y, -height / 2},
                             normal,
                             {float(i) / slices, 0},
                             tangent});
  }

  for (uint32_t i = 0; i < slices; i++) {
    mesh.indices.insert(mesh.indices.end(),
                        {tipIndex, baseStart + i, baseStart + i + 1});
  }

  addDisk(mesh, radius, {0, 0, -height / 2}, {0, 0, -1}, slices);
  CPUAsset::ModelData model;
  model.meshes.push_back(mesh);
  return model;
}

CPUAsset::ModelData CreatePyramid() {
  CPUAsset::MeshData mesh;
  mesh.materialIndex = 0;
  const float h = 0.5f;
  glm::vec3 top{0, 0, h};
  glm::vec3 base[4] = {{-h, -h, -h}, {h, -h, -h}, {h, h, -h}, {-h, h, -h}};

  uint32_t topIndex = mesh.vertices.size();
  mesh.vertices.push_back({top, {0, 0, 1}, {0.5f, 1}, {1, 0, 0}});

  uint32_t baseStart = mesh.vertices.size();
  for (int i = 0; i < 4; i++) {
    glm::vec3 e1 = base[(i + 1) % 4] - base[i];
    glm::vec3 e2 = top - base[i];
    glm::vec3 normal = glm::normalize(glm::cross(e1, e2));
    glm::vec3 tangent = glm::normalize(e1);
    mesh.vertices.push_back({base[i], normal, {float(i) / 4, 0}, tangent});
  }

  for (int i = 0; i < 4; i++)
    mesh.indices.insert(mesh.indices.end(),
                        {topIndex, baseStart + i, baseStart + (i + 1) % 4});

  uint32_t center = mesh.vertices.size();
  mesh.vertices.push_back({{0, 0, -h}, {0, 0, -1}, {0.5f, 0.5f}, {1, 0, 0}});
  for (int i = 0; i < 4; i++)
    mesh.indices.insert(mesh.indices.end(),
                        {center, baseStart + (i + 1) % 4, baseStart + i});

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
