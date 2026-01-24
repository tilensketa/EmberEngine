#pragma once

#include "IEvent.h"

#include <functional>

namespace Ember {
class EventDispatcher {
public:
  template <typename T> using EventFn = std::function<bool(T &)>;

  EventDispatcher(Event::IEvent &event) : mEvent(event) {}

  template <typename T> bool Dispatch(Event::EventType type, EventFn<T> func) {
    if (mEvent.GetType() == type && !mEvent.handled) {
      if (auto *e = dynamic_cast<T *>(&mEvent)) {
        mEvent.handled = func(*e);
        return true;
      }
    }
    return false;
  }

private:
  Event::IEvent &mEvent;
};
} // namespace Ember
