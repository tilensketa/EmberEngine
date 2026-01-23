#pragma once

#include "Core/Events/IEvent.h"

namespace Ember {
namespace Event {

struct MouseMove : public IEvent {
  double x, y;

  MouseMove(double x, double y) : x(x), y(y) {}
  EventType GetType() const override { return EventType::MOUSE_MOVE; }
  std::string ToString() const override {
    return "MOUSE_MOVE: x: " + std::to_string(x) + ", y: " + std::to_string(y);
  }
};

struct MouseButtonPress : public IEvent {
  int keycode;

  MouseButtonPress(int keycode) : keycode(keycode) {}
  EventType GetType() const override { return EventType::MOUSE_BUTTON_PRESS; }
  std::string ToString() const override {
    return "MOUSE_BUTTON_PRESS: keycode: " + std::to_string(keycode);
  }
};

struct MouseButtonRelease : public IEvent {
  int keycode;

  MouseButtonRelease(int keycode) : keycode(keycode) {}
  EventType GetType() const override { return EventType::MOUSE_BUTTON_RELEASE; }
  std::string ToString() const override {
    return "MOUSE_BUTTON_RELEASE: keycode: " + std::to_string(keycode);
  }
};

} // namespace Event
} // namespace Ember
