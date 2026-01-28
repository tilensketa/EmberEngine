#include "ReactPhysics.h"

#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

namespace Ember::Physics {

reactphysics3d::BodyType FromBodyType(const RigidbodyDesc::Type &type) {
  switch (type) {
  case RigidbodyDesc::Type::Static: return reactphysics3d::BodyType::STATIC;
  case RigidbodyDesc::Type::Dynamic: return reactphysics3d::BodyType::DYNAMIC;
  case RigidbodyDesc::Type::Kinematic:
    return reactphysics3d::BodyType::KINEMATIC;
  default: return reactphysics3d::BodyType::STATIC;
  }
}

void ReactPhysicsBody::SetTransform(const glm::vec3 &pos,
                                    const glm::quat &rot) {
  const reactphysics3d::Vector3 position(pos.x, pos.y, pos.z);
  const reactphysics3d::Quaternion rotation(rot.x, rot.y, rot.z, rot.w);
  const reactphysics3d::Transform transform(position, rotation);
  body->setTransform(transform);
}
void ReactPhysicsBody::GetTransform(glm::vec3 &pos, glm::quat &rot) const {
  const reactphysics3d::Transform &transform = body->getTransform();
  const reactphysics3d::Vector3 &p = transform.getPosition();
  const reactphysics3d::Quaternion &r = transform.getOrientation();
  pos = glm::vec3(p.x, p.y, p.z);
  rot = glm::quat(r.w, r.x, r.y, r.z);
}

ReactPhysicsWorld::ReactPhysicsWorld() {
  reactphysics3d::PhysicsWorld::WorldSettings settings;
  settings.gravity = reactphysics3d::Vector3(0.0f, 0.0f, -9.81f);
  settings.defaultPositionSolverNbIterations = 20;
  mWorld = mCommon.createPhysicsWorld(settings);
}
ReactPhysicsWorld::~ReactPhysicsWorld() {
  if (!mWorld) return;
  for (auto &body : mBodies) {
    mWorld->destroyRigidBody(body->body);
  }
  mCommon.destroyPhysicsWorld(mWorld);
  mWorld = nullptr;
}

void ReactPhysicsWorld::Step(float dt) {
  if (!mWorld) return;
  mWorld->update(dt);
}

IPhysicsBody *ReactPhysicsWorld::CreateBody(const glm::vec3 &pos,
                                            const glm::quat &rot,
                                            const RigidbodyDesc &rb) {
  if (!mWorld) return nullptr;

  reactphysics3d::Vector3 position(pos.x, pos.y, pos.z);
  reactphysics3d::Quaternion rotation(rot.x, rot.y, rot.z, rot.w);
  reactphysics3d::Transform physicsTransform(position, rotation);
  auto *body = mWorld->createRigidBody(physicsTransform);
  body->setType(FromBodyType(rb.type));

  switch (rb.type) {
  case RigidbodyDesc::Type::Dynamic:
    body->setMass(rb.mass);
    body->enableGravity(rb.useGravity);
    break;
  case RigidbodyDesc::Type::Static:
  case RigidbodyDesc::Type::Kinematic: body->enableGravity(false); break;
  }

  auto wrappedBody = std::make_unique<ReactPhysicsBody>();
  wrappedBody->body = body;

  IPhysicsBody *result = wrappedBody.get();
  mBodies.push_back(std::move(wrappedBody));
  return result;
}
void ReactPhysicsWorld::AddCollider(IPhysicsBody *body,
                                    const ColliderDesc &collider,
                                    const glm::vec3 &scale) {
  auto *reactBody = static_cast<ReactPhysicsBody *>(body);
  auto *native = reactBody->body;
  if (!native) return;

  reactphysics3d::CollisionShape *shape = nullptr;
  glm::vec3 rp3dScale(scale.x, scale.z, scale.y);
  switch (collider.type) {
  case ColliderDesc::Type::Box: {
    glm::vec3 halfExtents = 0.5f * collider.size * rp3dScale;
    shape =
        mCommon.createBoxShape({halfExtents.x, halfExtents.y, halfExtents.z});
    break;
  }
  case ColliderDesc::Type::Sphere: {
    float r = collider.radius * glm::compMax(rp3dScale);
    shape = mCommon.createSphereShape(r);
    break;
  }
  case ColliderDesc::Type::Capsule: {
    float r = collider.radius * glm::max(rp3dScale.x, rp3dScale.z);
    // TODO capsule collider and visualization dont match
    float h = collider.height * rp3dScale.y;
    shape = mCommon.createCapsuleShape(r, h);
    break;
  }
  }
  if (!shape) return;

  glm::quat yAxisRot = glm::quat(0.70710678118f, 0.70710678118f, 0.0f, 0.0f);
  glm::quat finalRot = collider.rotation * yAxisRot;
  reactphysics3d::Transform offset(
      {collider.offset.x, collider.offset.y, collider.offset.z},
      {finalRot.x, finalRot.y, finalRot.z, finalRot.w});
  auto coll = native->addCollider(shape, offset);
  coll->setIsTrigger(collider.isTrigger);
}

} // namespace Ember::Physics
