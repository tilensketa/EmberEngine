#include "LayerStack.h"

#include <algorithm>

namespace Ember {
LayerStack::~LayerStack() {
  for (auto layer : mLayers) {
    layer->OnDetach();
  }
}

void LayerStack::PushLayer(std::shared_ptr<ILayer> layer) {
  mLayers.push_back(layer);
  layer->OnAttach();
}
void LayerStack::PopLayer(std::shared_ptr<ILayer> layer) {
  mLayers.erase(std::remove(mLayers.begin(), mLayers.end(), layer),
                mLayers.end());
  layer->OnDetach();
}

void LayerStack::OnUpdate() {
  for (auto layer : mLayers) {
    layer->OnUpdate();
  }
}

void LayerStack::DispatchEvent(Event::IEvent &event) {
  for (auto layer : mLayers) {
    layer->OnEvent(event);
    if (event.handled) break;
  }
}
} // namespace Ember
