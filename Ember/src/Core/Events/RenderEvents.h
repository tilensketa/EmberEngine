#pragma once

#include "Core/Events/IEvent.h"
#include "Renderer/RenderContext.h"

namespace Ember {
namespace Event {

struct RenderReqeust : public IEvent {
  uint8_t id;
  RenderContext renderContext;

  RenderReqeust(uint8_t id, RenderContext renderContext)
      : id(id), renderContext(renderContext) {}
  EventType GetType() const override { return EventType::RENDER_REQUEST; }
  std::string ToString() const override {
    return "RENDER_REQUEST: id: " + std::to_string(id);
  }
};

struct RenderComplete : public IEvent {
  uint8_t id;
  RenderContext renderContext;

  RenderComplete(uint8_t id, RenderContext renderContext)
      : id(id), renderContext(renderContext) {}
  EventType GetType() const override { return EventType::RENDER_COMPLETE; }
  std::string ToString() const override {
    return "RENDER_COMPLETE: id: " + std::to_string(id);
  }
};

} // namespace Event
} // namespace Ember
