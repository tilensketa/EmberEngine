#pragma once

#include "Core/Events/IEvent.h"

namespace Ember {
namespace Event {

struct WindowResize : public IEvent {
  uint32_t width;
  uint32_t height;

  WindowResize(uint32_t width, uint32_t height)
      : width(width), height(height) {}
  EventType GetType() const override { return EventType::WINDOW_RESIZE; }
  std::string ToString() const override {
    return "WINDOW_RESIZE: width: " + std::to_string(width) +
           ", height: " + std::to_string(width);
  }
};
} // namespace Event
} // namespace Ember
