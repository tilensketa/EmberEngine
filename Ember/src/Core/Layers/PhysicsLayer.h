#pragma once

#include "ILayer.h"
#include "Project/Scene/Scene.h"

#include <memory>

#include <reactphysics3d/reactphysics3d.h>

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
  reactphysics3d::PhysicsCommon mCommon;
  std::shared_ptr<Scene> mScene = nullptr;
  bool mSimulate = false;
};
} // namespace Ember
