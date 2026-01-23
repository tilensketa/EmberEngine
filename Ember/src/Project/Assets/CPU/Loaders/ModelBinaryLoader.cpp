#include "ModelBinaryLoader.h"

#include "Core/Logger.h"
#include "Core/Serialization.h"
#include "Project/FileTypes.h"

#include <yaml-cpp/yaml.h>

namespace Ember::ModelBinaryLoader {

std::optional<ModelBinaryData>
FromBinaryFile(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) return std::nullopt;

  ModelBinaryData modelBinaryData;
  YAML::Node root = YAML::LoadFile(path);
  YAML::Node meshesNode = root["meshes"];
  for (const auto &meshNode : meshesNode) {
    MeshBinaryData meshBinaryData;
    for (const auto &vertNode : meshNode["vertices"]) {
      meshBinaryData.vertices.push_back(
          vertNode.as<CPUAsset::MeshData::Vertex>());
    }
    for (const auto &indNode : meshNode["indices"]) {
      meshBinaryData.indices.push_back(indNode.as<GLuint>());
    }
    meshBinaryData.material = meshNode["material"].as<GUID>();
    modelBinaryData.meshes.push_back(meshBinaryData);
  }
  return modelBinaryData;
}
void ToBinaryFile(const ModelBinaryData &modelBinaryData,
                  const std::filesystem::path &path) {
  YAML::Node root;
  for (const auto &meshBinaryData : modelBinaryData.meshes) {
    YAML::Node meshNode;
    for (const auto &vert : meshBinaryData.vertices) {
      meshNode["vertices"].push_back(vert);
    }
    for (const auto &ind : meshBinaryData.indices) {
      meshNode["indices"].push_back(ind);
    }
    meshNode["material"] = meshBinaryData.material;
    root["meshes"].push_back(meshNode);
  }

  std::ofstream(path) << root;
}

} // namespace Ember::ModelBinaryLoader

namespace YAML {

template <> struct convert<Ember::CPUAsset::MeshData::Vertex> {
  static Node encode(const Ember::CPUAsset::MeshData::Vertex &vertex) {
    Node node;
    node["position"] = vertex.position;
    node["normal"] = vertex.normal;
    node["tex_coord"] = vertex.texCoord;
    node["tangent"] = vertex.tangent;
    return node;
  }
  static bool decode(const Node &node,
                     Ember::CPUAsset::MeshData::Vertex &vertex) {
    vertex.position = node["position"].as<glm::vec3>();
    vertex.normal = node["normal"].as<glm::vec3>();
    vertex.texCoord = node["tex_coord"].as<glm::vec2>();
    vertex.tangent = node["tangent"].as<glm::vec3>();
    return true;
  }
};
} // namespace YAML
