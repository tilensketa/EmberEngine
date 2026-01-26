#pragma once

#include "Project/ProjectManager.h"
#include "RenderContext.h"

#include "Buffers/UBO.h"
#include "Renderer/Shader.h"
#include <memory>

namespace Ember {

// Lights
struct GPULight {
  glm::vec4 position; // xyz + type
  glm::vec4 color;    // rgb
  glm::vec4 props;    // intensity + radius
};
struct LightBlockCPU {
  glm::ivec4 lightCount;
  GPULight lights[128];
};

class Renderer {
public:
  Renderer(ProjectManager &projectManager, AssetRegistry &assetRegistry);
  ~Renderer() = default;

  void Render(const RenderContext &ctx);

private:
  void collectLights(const RenderContext &ctx);
  void renderModels(const RenderContext &ctx, Shader* shader);
  void renderLights(const RenderContext &ctx, Shader* shader);
  void renderColliders(const RenderContext &ctx, Shader* shader);
  void bindTexture(const GUID &guid, const GLenum &textureId,
                   const std::string &name);

private:
  std::unique_ptr<Shader> mShader;
  std::unique_ptr<Shader> mFlatShader;
  ProjectManager &mProjectManager;
  AssetRegistry &mAssetRegistry;
  std::unique_ptr<UBO> mLightUBO;
};
} // namespace Ember
