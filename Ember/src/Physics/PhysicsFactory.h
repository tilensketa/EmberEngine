#pragma once

#include "IPhysicsBackend.h"
#include "react/ReactPhysics.h"

#include <memory.h>

namespace Ember::Physics {
class PhysicsFactory {
public:
  std::unique_ptr<IPhysicsWorld> CreateWorld() {
    return std::make_unique<ReactPhysicsWorld>();
  }
};
} // namespace Ember::Physics
