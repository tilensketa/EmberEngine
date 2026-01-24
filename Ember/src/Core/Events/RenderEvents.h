#pragma once

#include "Core/Events/IEvent.h"
#include "Renderer/RenderContext.h"

namespace Ember {
namespace Event {

struct RenderRequest : public IEvent {
  RenderContext renderContext;

  RenderRequest(RenderContext renderContext) : renderContext(renderContext) {}
  EventType GetType() const override { return EventType::RENDER_REQUEST; }
  std::string ToString() const override { return "RENDER_REQUEST"; }
};

} // namespace Event
} // namespace Ember
