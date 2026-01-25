#pragma once

#include "Core/Events/IEvent.h"

#include "Project/Scene/Scene.h"

namespace Ember {
namespace Event {

struct PhysicsStart : public IEvent {
  std::shared_ptr<Scene> scene;
  PhysicsStart(std::shared_ptr<Scene> scene) : scene(scene) {}
  EventType GetType() const override { return EventType::PHYSICS_START; }
  std::string ToString() const override { return "PHYSICS_START"; }
};
struct PhysicsStop : public IEvent {
  PhysicsStop() {}
  EventType GetType() const override { return EventType::PHYSICS_STOP; }
  std::string ToString() const override { return "PHYSICS_STOP"; }
};
struct PhysicsPause : public IEvent {
  PhysicsPause() {}
  EventType GetType() const override { return EventType::PHYSICS_PAUSE; }
  std::string ToString() const override { return "PHYSICS_PAUSE"; }
};
struct PhysicsUnPause : public IEvent {
  PhysicsUnPause() {}
  EventType GetType() const override { return EventType::PHYSICS_UNPAUSE; }
  std::string ToString() const override { return "PHYSICS_UNPAUSE"; }
};

} // namespace Event
} // namespace Ember
