#include "PhysicsLayer.h"

#include "Core/Events/Events.h"

#include "Project/Scene/Components.h"

namespace Ember {

reactphysics3d::Vector3 FromGlmVec3(const glm::vec3 &v) {
  return reactphysics3d::Vector3(v.x, v.y, v.z);
}
glm::vec3 FromReactVec3(const reactphysics3d::Vector3 &v) {
  return glm::vec3(v.x, v.y, v.z);
}
reactphysics3d::Quaternion FromGlmQuat(const glm::quat &q) {
  return reactphysics3d::Quaternion(q.x, q.y, q.z, q.w);
}
glm::quat FromReactQuat(const reactphysics3d::Quaternion &q) {
  return glm::quat(q.w, q.x, q.y, q.z);
}

reactphysics3d::BodyType
FromBodyType(const Component::Rigidbody::BodyType &type) {
  switch (type) {
  case Component::Rigidbody::BodyType::Static:
    return reactphysics3d::BodyType::STATIC;
  case Component::Rigidbody::BodyType::Dynamic:
    return reactphysics3d::BodyType::DYNAMIC;
  case Component::Rigidbody::BodyType::Kinematic:
    return reactphysics3d::BodyType::KINEMATIC;
  default: return reactphysics3d::BodyType::STATIC;
  }
}

void PhysicsLayer::OnAttach() {}
void PhysicsLayer::OnDetach() {}
void PhysicsLayer::OnUpdate() {
  if (!mScene || !mSimulate) return;

  auto world = mScene->GetPhysicsWorld();
  constexpr float physicsStep = 1.0f / 60.0f;
  world->update(physicsStep);
  auto physicsView = mScene->View<Component::Transform, Component::Rigidbody>();
  for (const auto physicsEntity : physicsView) {
    auto &transform = mScene->Get<Component::Transform>(physicsEntity);
    auto &rigidbody = mScene->Get<Component::Rigidbody>(physicsEntity);
    auto body = rigidbody.physicsBodyHandle;
    if (!body) continue;

    if (rigidbody.bodyType == Component::Rigidbody::BodyType::Dynamic) {
      transform.position = FromReactVec3(body->getTransform().getPosition());
      transform.rotation = FromReactQuat(body->getTransform().getOrientation());
    }
    if (rigidbody.bodyType == Component::Rigidbody::BodyType::Kinematic) {
      reactphysics3d::Transform physicsTransform(
          FromGlmVec3(transform.position), FromGlmQuat(transform.rotation));
      body->setTransform(physicsTransform);
    }
  }
}
void PhysicsLayer::OnEvent(Event::IEvent &event) {
  EventDispatcher dispatcher(event);

  dispatcher.Dispatch<Event::PhysicsStart>(Event::EventType::PHYSICS_START,
                                           [this](Event::PhysicsStart &e) {
                                             mScene = e.scene;
                                             createWorld();
                                             mSimulate = true;
                                             return true;
                                           });
  dispatcher.Dispatch<Event::PhysicsStop>(Event::EventType::PHYSICS_STOP,
                                          [this](Event::PhysicsStop &e) {
                                            if (!mScene) return true;
                                            destroyWorld();
                                            mScene = nullptr;
                                            mSimulate = false;
                                            return true;
                                          });
  dispatcher.Dispatch<Event::PhysicsPause>(Event::EventType::PHYSICS_PAUSE,
                                           [this](Event::PhysicsPause &e) {
                                             mSimulate = false;
                                             return true;
                                           });
  dispatcher.Dispatch<Event::PhysicsUnPause>(Event::EventType::PHYSICS_UNPAUSE,
                                             [this](Event::PhysicsUnPause &e) {
                                               mSimulate = true;
                                               return true;
                                             });
}

void PhysicsLayer::createWorld() {
  if (!mScene) return;

  reactphysics3d::PhysicsWorld::WorldSettings settings;
  settings.defaultVelocitySolverNbIterations = 20;
  settings.isSleepingEnabled = true;
  settings.gravity =
      reactphysics3d::Vector3(0, 0, reactphysics3d::decimal(-9.81));
  auto world = mCommon.createPhysicsWorld(settings);
  mScene->SetPhysicsWorld(world);

  auto physicsView = mScene->View<Component::Transform, Component::Rigidbody>();
  for (const auto physicsEntity : physicsView) {
    auto &transform = mScene->Get<Component::Transform>(physicsEntity);
    auto &rigidbody = mScene->Get<Component::Rigidbody>(physicsEntity);

    reactphysics3d::Transform physicsTransform(FromGlmVec3(transform.position),
                                               FromGlmQuat(transform.rotation));
    auto *body = world->createRigidBody(physicsTransform);
    body->setType(FromBodyType(rigidbody.bodyType));

    switch (rigidbody.bodyType) {
    case Component::Rigidbody::BodyType::Dynamic:
      body->setMass(rigidbody.mass);
      body->enableGravity(rigidbody.useGravity);
      break;

    case Component::Rigidbody::BodyType::Static:
    case Component::Rigidbody::BodyType::Kinematic:
      body->enableGravity(false);
      break;
    }
    rigidbody.physicsBodyHandle = body;

    if (mScene->HasComponent<Component::Collider>(physicsEntity)) {
      auto &collider = mScene->Get<Component::Collider>(physicsEntity);
      glm::quat defaultRot;
      defaultRot.w = 0.70710678118f;
      defaultRot.x = 0.70710678118f;
      defaultRot.y = 0.0f;
      defaultRot.z = 0.0f;
      reactphysics3d::Transform offsetTransform(FromGlmVec3(collider.offset),
                                                FromGlmQuat(defaultRot));
      // FromGlmQuat(collider.rotation));
      auto transScale =
          glm::vec3(transform.scale.x, transform.scale.z, transform.scale.y);
      reactphysics3d::CollisionShape *shape = nullptr;
      if (collider.colliderType == Component::Collider::ColliderType::Box) {
        auto collScale = glm::vec3(collider.boxScale.x, collider.boxScale.z,
                                   collider.boxScale.y);
        shape = mCommon.createBoxShape(
            FromGlmVec3((collScale * transScale) * 0.5f));
      } else if (collider.colliderType ==
                 Component::Collider::ColliderType::Sphere) {
        shape = mCommon.createSphereShape(collider.radius *
                                          glm::compMax(transScale) * 0.5f);
      } else if (collider.colliderType ==
                 Component::Collider::ColliderType::Capsule) {
        float maxRadius = glm::max(transScale.x, transScale.z);
        shape =
            mCommon.createCapsuleShape(collider.radius * maxRadius * 0.5f,
                                       collider.height * transScale.y * 0.5f);
      }
      if (shape) body->addCollider(shape, offsetTransform);
    }
  }
}
void PhysicsLayer::destroyWorld() {
  if (!mScene) return;

  auto physicsView = mScene->View<Component::Rigidbody>();
  for (const auto physicsEntity : physicsView) {
    auto &rigidbody = mScene->Get<Component::Rigidbody>(physicsEntity);
    rigidbody.physicsBodyHandle = nullptr;
  }
  mCommon.destroyPhysicsWorld(mScene->GetPhysicsWorld());
  mScene->SetPhysicsWorld(nullptr);
}
} // namespace Ember
