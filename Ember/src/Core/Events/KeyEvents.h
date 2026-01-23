#pragma once

#include "Core/Events/IEvent.h"

namespace Ember {
namespace Event {

struct KeyPress : public IEvent {
  int keycode;

  KeyPress(int keycode) : keycode(keycode) {}
  EventType GetType() const override { return EventType::KEY_PRESS; }
  std::string ToString() const override {
    return "KEY_PRESS: keycode: " + std::to_string(keycode);
  }
};

struct KeyRelease : public IEvent {
  int keycode;

  KeyRelease(int keycode) : keycode(keycode) {}
  EventType GetType() const override { return EventType::KEY_RELEASE; }
  std::string ToString() const override {
    return "KEY_RELEASE: keycode: " + std::to_string(keycode);
  }
};

struct KeyRepeat : public IEvent {
  int keycode;

  KeyRepeat(int keycode) : keycode(keycode) {}
  EventType GetType() const override { return EventType::KEY_REPEAT; }
  std::string ToString() const override {
    return "KEY_REPEAT: keycode: " + std::to_string(keycode);
  }
};

} // namespace Event
} // namespace Ember
