#include "PhysicsLayer.h"

#include "Core/Events/Events.h"

#include "Physics/Conversions.h"
#include "Project/Scene/Components.h"

namespace Ember {

void PhysicsLayer::OnAttach() {}
void PhysicsLayer::OnDetach() {}
void PhysicsLayer::OnUpdate() {
  if (!mScene || !mSimulate) return;

  auto world = mScene->GetPhysicsWorld();
  constexpr float physicsStep = 1.0f / 60.0f;
  world->Step(physicsStep);

  auto physicsView = mScene->View<Component::Transform, Component::Rigidbody>();
  for (const auto physicsEntity : physicsView) {
    auto &transform = mScene->Get<Component::Transform>(physicsEntity);
    auto &rigidbody = mScene->Get<Component::Rigidbody>(physicsEntity);
    auto body = rigidbody.physicsBodyHandle;
    if (!body) continue;

    if (rigidbody.bodyType == Component::Rigidbody::BodyType::Dynamic) {
      body->GetTransform(transform.position, transform.rotation);
    } else if (rigidbody.bodyType ==
               Component::Rigidbody::BodyType::Kinematic) {
      body->SetTransform(transform.position, transform.rotation);
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

  mScene->SetPhysicsWorld(mPhysicsFactory.CreateWorld());
  auto world = mScene->GetPhysicsWorld();

  auto physicsView = mScene->View<Component::Transform, Component::Rigidbody>();
  for (const auto physicsEntity : physicsView) {
    auto &transform = mScene->Get<Component::Transform>(physicsEntity);
    auto &rigidbody = mScene->Get<Component::Rigidbody>(physicsEntity);

    Physics::RigidbodyDesc rbDesc;
    rbDesc.type = Physics::FromRbType(rigidbody.bodyType);
    rbDesc.mass = rigidbody.mass;
    rbDesc.useGravity = rigidbody.useGravity;
    auto body =
        world->CreateBody(transform.position, transform.rotation, rbDesc);
    rigidbody.physicsBodyHandle = body;

    if (mScene->HasComponent<Component::Collider>(physicsEntity)) {
      const auto &collider = mScene->Get<Component::Collider>(physicsEntity);

      Physics::ColliderDesc cDesc;
      cDesc.type = Physics::FromCollType(collider.colliderType);
      cDesc.rotation = collider.rotation;
      cDesc.offset = collider.offset;
      cDesc.size = collider.boxScale;
      cDesc.height = collider.height;
      cDesc.radius = collider.radius;
      cDesc.isTrigger = collider.isTrigger;
      world->AddCollider(body, cDesc, transform.scale);
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
  mScene->DestroyPhysicsWorld();
}
} // namespace Ember
