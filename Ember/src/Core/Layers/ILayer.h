#pragma once

#include "Core/Events/IEvent.h"

#include <memory>

namespace Ember {
class ILayer {
public:
  virtual ~ILayer() = default;

  virtual void OnAttach() {};
  virtual void OnDetach() {};
  virtual void OnUpdate() {};
  virtual void OnEvent(Event::IEvent &event) {};
};
} // namespace Ember
