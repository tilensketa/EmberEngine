#pragma once

#include "ILayer.h"
#include <memory>
#include <vector>

namespace Ember {
class LayerStack {
public:
  LayerStack() = default;
  ~LayerStack();

  void PushLayer(std::shared_ptr<ILayer> layer);
  void PopLayer(std::shared_ptr<ILayer> layer);

  void OnUpdate();
  void DispatchEvent(Event::IEvent& event);

private:
  std::vector<std::shared_ptr<ILayer>> mLayers;
};
} // namespace Ember
