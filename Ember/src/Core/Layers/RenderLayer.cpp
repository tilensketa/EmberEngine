#include "RenderLayer.h"
#include "Core/Events/RenderEvents.h"

#include "Core/Application.h"

namespace Ember {
RenderLayer::RenderLayer(ProjectManager &projectManager,
                         AssetRegistry &assetRegistry) {
  mRenderer = std::make_unique<Renderer>(projectManager, assetRegistry);
}
RenderLayer::~RenderLayer() {}

void RenderLayer::OnAttach() {}
void RenderLayer::OnDetach() {}
void RenderLayer::OnUpdate() {}
void RenderLayer::OnEvent(Event::IEvent &event) {
  if (event.GetType() == Event::EventType::RENDER_REQUEST) {
    if (auto *renderRequestEvent =
            dynamic_cast<Event::RenderReqeust *>(&event)) {
      RenderContext renderContext = renderRequestEvent->renderContext;
      uint8_t id = renderRequestEvent->id;
      mRenderer->Render(renderContext);
      Event::RenderComplete renderCompleteEvent(id, renderContext);
    }
    event.handled = true;
  }
}

} // namespace Ember
