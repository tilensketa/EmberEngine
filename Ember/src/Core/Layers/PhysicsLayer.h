#pragma once

#include "ILayer.h"
#include "Project/Scene/Scene.h"

#include <memory>

#include "Physics/PhysicsFactory.h"

namespace Ember {
class PhysicsLayer : public ILayer {
public:
  PhysicsLayer() = default;
  ~PhysicsLayer() = default;

  virtual void OnAttach() override;
  virtual void OnDetach() override;
  virtual void OnUpdate() override;
  virtual void OnEvent(Event::IEvent &event) override;

private:
  void createWorld();
  void destroyWorld();

private:
  Physics::PhysicsFactory mPhysicsFactory;
  std::shared_ptr<Scene> mScene = nullptr;
  bool mSimulate = false;
};
} // namespace Ember
