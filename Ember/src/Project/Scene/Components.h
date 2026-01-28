#pragma once

#include "Core/GUID.h"
#include "Core/Utils.h"
#include "Project/Assets/AssetHandle.h"

#include "Physics/IPhysicsBackend.h"

#include <glm/glm.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <optional>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Ember {
namespace Component {
struct Id {
  GUID guid;
};
struct Tag {
  std::string value;
};
struct Transform {
  glm::vec3 position = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(1, 0, 0, 0);
  glm::vec3 scale = glm::vec3(1.0f);

  glm::mat4 GetMatrix() const {
    return glm::translate(glm::mat4(1.0f), position) * glm::toMat4(rotation) *
           glm::scale(glm::mat4(1.0f), scale);
  }
  void SetMatrix(const glm::mat4 &mat) {
    glm::vec3 skew;
    glm::vec4 persp;
    glm::decompose(mat, scale, rotation, position, skew, persp);
  }
};
struct Model {
  std::optional<AssetHandle> model = std::nullopt;
  std::vector<GUID> materialOverrides;
};
struct Camera {
  enum class Projection : uint8_t { Perspective = 0, Orthographic };
  enum class ViewMode : uint8_t {
    Wireframe = 0,
    Solid,
    Diffuse,
    Normal,
    Metalness,
    Roughness,
    TexCoords,
    Rendered,
    Debug
  };

  Projection projection = Projection::Perspective;
  ViewMode viewMode = ViewMode::Rendered;
  glm::uvec2 resolution = glm::uvec2(1920, 1080);
  float aspect = float(resolution.x) / float(resolution.y);
  float orthoSize = 2.0f;
  float near = 0.1f;
  float far = 100000.0f;
  float fov = 45.0f;
  bool active = true;

  glm::mat4 GetProjection() const {
    glm::mat4 projectionMatrix = glm::mat4(1.0f);
    if (projection == Projection::Perspective) {
      projectionMatrix = glm::perspective(glm::radians(fov), aspect, near, far);
    } else if (projection == Projection::Orthographic) {
      float y = orthoSize;
      float x = aspect * orthoSize;
      projectionMatrix = glm::ortho(-x, x, -y, y, near, far);
    }
    return projectionMatrix;
  }
};

struct Light {
  enum class LightType : uint8_t { Point = 0, Directional };

  LightType lightType = LightType::Point;
  glm::vec3 color = glm::vec3(1.0f);
  float intensity = 10.0f;
  float radius = 5.0f;
};

struct Rigidbody {
  enum class BodyType : uint8_t { Static = 0, Dynamic, Kinematic };

  BodyType bodyType = BodyType::Static;
  float mass = 1.0f;
  bool useGravity = true;

  Physics::IPhysicsBody *physicsBodyHandle = nullptr;
};

struct Collider {
  enum class ColliderType : uint8_t {
    Box = 0,
    Sphere,
    Capsule
  }; // TODO add mesh

  ColliderType colliderType = ColliderType::Box;
  glm::vec3 offset = glm::vec3(0.0f);
  glm::quat rotation = glm::quat(1, 0, 0, 0);

  glm::vec3 boxScale = glm::vec3(1.0f);
  float radius = 1.0f;
  float height = 2.0f;

  bool isTrigger = false;
};

using AllComponents =
    std::tuple<Id, Tag, Transform, Model, Camera, Light, Rigidbody, Collider>;

#define REGISTER_COMPONENT_NAME(TYPE, NAME)                                    \
  template <> constexpr const char *ComponentName<TYPE>() { return NAME; }

template <typename T> constexpr const char *ComponentName();

REGISTER_COMPONENT_NAME(Component::Id, "Id")
REGISTER_COMPONENT_NAME(Component::Tag, "Tag")
REGISTER_COMPONENT_NAME(Component::Transform, "Transform")
REGISTER_COMPONENT_NAME(Component::Model, "Model")
REGISTER_COMPONENT_NAME(Component::Camera, "Camera")
REGISTER_COMPONENT_NAME(Component::Light, "Light")
REGISTER_COMPONENT_NAME(Component::Rigidbody, "Rigidbody")
REGISTER_COMPONENT_NAME(Component::Collider, "Collider")
} // namespace Component
} // namespace Ember

namespace Ember::Utils {
template <> struct EnumTraits<Component::Camera::Projection> {
  inline static constexpr std::array<const char *, 2> Names = {"Perspective",
                                                               "Orthographic"};
};
template <> struct EnumTraits<Component::Camera::ViewMode> {
  inline static constexpr std::array<const char *, 9> Names = {
      "Wireframe", "Solid",     "Albedo",   "Normal", "Metalness",
      "Roughness", "TexCoords", "Rendered", "Debug"};
};
template <> struct EnumTraits<Component::Light::LightType> {
  inline static constexpr std::array<const char *, 2> Names = {"Point",
                                                               "Directional"};
};
template <> struct EnumTraits<Component::Rigidbody::BodyType> {
  inline static constexpr std::array<const char *, 3> Names = {
      "Static", "Dynamic", "Kinematic"};
};
template <> struct EnumTraits<Component::Collider::ColliderType> {
  inline static constexpr std::array<const char *, 3> Names = {"Box", "Sphere",
                                                               "Capsule"};
};
} // namespace Ember::Utils
