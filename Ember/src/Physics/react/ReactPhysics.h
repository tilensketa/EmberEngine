#pragma once

#include "Physics/IPhysicsBackend.h"

#include <reactphysics3d/reactphysics3d.h>

namespace Ember::Physics {

class ReactPhysicsBody : public IPhysicsBody {
public:
  reactphysics3d::RigidBody *body;

  void SetTransform(const glm::vec3 &pos, const glm::quat &rot) override;
  void GetTransform(glm::vec3 &pos, glm::quat &rot) const override;
};

class ReactPhysicsWorld : public IPhysicsWorld {
public:
  ReactPhysicsWorld();
  ~ReactPhysicsWorld() override;

  void Step(float dt) override;

  IPhysicsBody *CreateBody(const glm::vec3 &pos, const glm::quat &rot,
                           const RigidbodyDesc &rb) override;
  void AddCollider(IPhysicsBody *body, const ColliderDesc &collider,
                   const glm::vec3 &scale) override;

private:
  reactphysics3d::PhysicsCommon mCommon;
  reactphysics3d::PhysicsWorld *mWorld = nullptr;
  std::vector<std::unique_ptr<ReactPhysicsBody>> mBodies;
};

} // namespace Ember::Physics
