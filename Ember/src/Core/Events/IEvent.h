#pragma once

#include <cstdint>
#include <string>

namespace Ember {
namespace Event {
enum class EventType : uint8_t {
  NONE = 0,

  WINDOW_RESIZE,

  RENDER_REQUEST,

  KEY_PRESS,
  KEY_RELEASE,
  KEY_REPEAT,

  MOUSE_MOVE,
  MOUSE_BUTTON_PRESS,
  MOUSE_BUTTON_RELEASE,

  PROJECT_OPEN,
  PROJECT_SAVE,
  PROJECT_CREATE,
  PROJECT_CLOSE,

  PHYSICS_START,
  PHYSICS_STOP,
  PHYSICS_UNPAUSE,
  PHYSICS_PAUSE
};
class IEvent {
public:
  bool handled = false;
  virtual ~IEvent() = default;
  virtual EventType GetType() const = 0;
  virtual std::string ToString() const { return "IEvent"; }
};
} // namespace Event
} // namespace Ember
