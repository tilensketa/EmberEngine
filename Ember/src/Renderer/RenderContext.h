#pragma once

#include "Project/Scene/Scene.h"
#include "RenderTarget.h"

namespace Ember {
struct RenderContext {
  RenderTarget* renderTarget;
  std::shared_ptr<Scene> scene = nullptr;
  Component::Camera camera;
  Component::Transform cameraTransform;
};
} // namespace Ember
