#pragma once

#include "Physics/IPhysicsBackend.h"
#include "Project/Scene/Components.h"

namespace Ember::Physics {

inline Physics::RigidbodyDesc::Type
FromRbType(const Component::Rigidbody::BodyType &type) {
  switch (type) {
  case Component::Rigidbody::BodyType::Static:
    return Physics::RigidbodyDesc::Type::Static;
  case Component::Rigidbody::BodyType::Dynamic:
    return Physics::RigidbodyDesc::Type::Dynamic;
  case Component::Rigidbody::BodyType::Kinematic:
    return Physics::RigidbodyDesc::Type::Kinematic;
  default: return Physics::RigidbodyDesc::Type::Static;
  }
}
inline Physics::ColliderDesc::Type
FromCollType(const Component::Collider::ColliderType &type) {
  switch (type) {
  case Component::Collider::ColliderType::Box:
    return Physics::ColliderDesc::Type::Box;
  case Component::Collider::ColliderType::Sphere:
    return Physics::ColliderDesc::Type::Sphere;
  case Component::Collider::ColliderType::Capsule:
    return Physics::ColliderDesc::Type::Capsule;
  default: return Physics::ColliderDesc::Type::Box;
  }
}
} // namespace Ember::Physics
