#include "RenderLayer.h"

#include "Core/Events/Events.h"

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
  EventDispatcher dispatcher(event);
  dispatcher.Dispatch<Event::RenderRequest>(
      Event::EventType::RENDER_REQUEST, [this](Event::RenderRequest &e) {
        mRenderer->Render(e.renderContext);
        return true;
      });
}

} // namespace Ember
