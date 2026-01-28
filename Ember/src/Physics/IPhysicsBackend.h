#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Ember::Physics {

struct RigidbodyDesc {
  enum class Type  { Static, Dynamic, Kinematic };
  Type type;
  float mass;
  bool useGravity;
};

struct ColliderDesc {
  enum class Type { Box, Sphere, Capsule };

  Type type;
  glm::quat rotation;
  glm::vec3 offset;
  glm::vec3 size;
  float radius;
  float height;
  bool isTrigger;
};

class IPhysicsBody {
public:
  virtual ~IPhysicsBody() = default;

  virtual void SetTransform(const glm::vec3 &pos, const glm::quat &rot) = 0;
  virtual void GetTransform(glm::vec3 &pos, glm::quat &rot) const = 0;
};

class IPhysicsWorld {
public:
  virtual ~IPhysicsWorld() = default;

  virtual void Step(float dt) = 0;

  virtual IPhysicsBody *CreateBody(const glm::vec3 &pos, const glm::quat &rot,
                                   const RigidbodyDesc &rb) = 0;
  virtual void AddCollider(IPhysicsBody *body, const ColliderDesc &collider,
                           const glm::vec3 &scale) = 0;
};

} // namespace Ember::Physics
