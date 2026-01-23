#include "Renderer.h"

#include "Project/Assets/Assets.h"
#include "Project/Assets/EngineAssetLoader.h"
#include "Project/Scene/Components.h"

#include "Core/Input.h"
#include "Renderer/Buffers/UBO.h"

#include <glm/gtc/matrix_transform.hpp>

#define SHADERS "../../Ember/res/shaders/"

namespace Ember {

Renderer::Renderer(ProjectManager &projectManager, AssetRegistry &assetRegistry)
    : mProjectManager(projectManager), mAssetRegistry(assetRegistry) {
  LOG_INFO("Renderer::Init: Initializing renderer");
  mShader = std::make_unique<Shader>(SHADERS, "shader.vert", "shader.frag");
  mLightUBO = std::make_unique<UBO>(sizeof(LightBlockCPU), 0);
}

void Renderer::Render(const RenderContext &ctx) {
  if (!ctx.renderTarget || !mShader) return;
  ctx.renderTarget->Bind();
  glViewport(0, 0, ctx.renderTarget->GetWidth(), ctx.renderTarget->GetHeight());

  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glm::mat4 axisMatrix(0.0f);
  axisMatrix[0][2] = 1.0f;
  axisMatrix[1][0] = 1.0f;
  axisMatrix[2][1] = -1.0f;
  axisMatrix[3][3] = 1.0f;
  auto cameraMatrix = ctx.camera.GetProjection() * axisMatrix *
                      glm::inverse(ctx.cameraTransform.GetMatrix());

  mShader->Activate();
  mShader->SetMat4("uCamMatrix", cameraMatrix);
  mShader->SetInt("uViewMode", static_cast<int>(ctx.camera.viewMode));
  mShader->SetVec3("uCameraPos", ctx.cameraTransform.position);

  mShader->SetFloat("uTime", Input::GetTime());

  if (ctx.scene) {
    // LIGHTS
    auto lightView = ctx.scene->View<Component::Transform, Component::Light>();
    LightBlockCPU lightBlock;
    lightBlock.lightCount.x = lightView.size();
    int i = 0;
    for (auto entity : lightView) {
      if (i >= 128) break;
      auto &transform = ctx.scene->Get<Component::Transform>(entity);
      auto &light = ctx.scene->Get<Component::Light>(entity);
      // TODO dont update lights in renderer
      transform.scale = glm::vec3(light.radius);
      const auto &pos = transform.position;
      const auto &col = light.color;

      lightBlock.lights[i].position =
          glm::vec4(pos.x, pos.y, pos.z, light.lightType);
      lightBlock.lights[i].color = glm::vec4(col.r, col.g, col.b, 0.0f);
      lightBlock.lights[i].props =
          glm::vec4(light.intensity, light.radius, 0, 0);

      if (ctx.camera.viewMode == Component::Camera::ViewMode::Debug) {
        if (light.lightType == Component::Light::LightType::Point) {
          AssetHandle h = EngineAssetLoader::GetSphereHandle();
          auto m = mAssetRegistry.GetAsset<Asset::Model>(h.guid);

          if (m) {
            mShader->SetMat4("uModel", transform.GetMatrix());
            m->meshes[0].gpu.vao->Bind();
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawElements(GL_TRIANGLES, m->meshes[0].gpu.indexCount,
                           GL_UNSIGNED_INT, 0);
            m->meshes[0].gpu.vao->Unbind();
          }
        }
      }

      i++;
    }
    mLightUBO->SetSubData(0, sizeof(LightBlockCPU), &lightBlock);

    // MODELS
    auto modelView = ctx.scene->View<Component::Transform, Component::Model>();
    for (const auto entity : modelView) {
      const auto &transform = ctx.scene->Get<Component::Transform>(entity);
      const auto &model = ctx.scene->Get<Component::Model>(entity);
      if (!model.model) continue;
      mShader->SetMat4("uModel", transform.GetMatrix());

      const GUID &guid = model.model->guid;
      auto modelAsset = mAssetRegistry.GetAsset<Asset::Model>(guid);
      for (size_t i = 0; i < modelAsset->meshes.size(); i++) {
        const auto &mesh = modelAsset->meshes[i];
        const GUID &meshMatGuid = mesh.material;
        const GUID &overrideMatGuid = model.materialOverrides[i];
        GUID matGuid =
            (overrideMatGuid == GUID::NONE()) ? meshMatGuid : overrideMatGuid;
        if (matGuid == GUID::NONE()) continue;
        const auto material = mAssetRegistry.GetAsset<Asset::Material>(matGuid);

        bindTexture(material->albedoMap, GL_TEXTURE0, "uHasAlbedoMap");
        bindTexture(material->normalMap, GL_TEXTURE1, "uHasNormalMap");
        bindTexture(material->metalnessMap, GL_TEXTURE2, "uHasMetalnessMap");
        bindTexture(material->roughnessMap, GL_TEXTURE3, "uHasRoughnessMap");

        mShader->SetVec3("uAlbedoColor", material->factors.albedoColor);
        mShader->SetFloat("uEmission", material->factors.emission);
        mShader->SetFloat("uMetalness", material->factors.metalness);
        mShader->SetFloat("uRoughness", material->factors.roughness);

        mesh.gpu.vao->Bind();
        if (ctx.camera.viewMode == Component::Camera::ViewMode::Wireframe)
          glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
          glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDrawElements(GL_TRIANGLES, mesh.gpu.indexCount, GL_UNSIGNED_INT, 0);
        mesh.gpu.vao->Unbind();
      }
    }
  }
  ctx.renderTarget->Unbind();
}
void Renderer::bindTexture(const GUID &guid, const GLenum &textureId,
                           const std::string &name) {
  // TODO fix null texture rendering
  if (guid != GUID::NONE()) {
    auto texture = mAssetRegistry.GetAsset<Asset::Texture>(guid);
    if (texture->gpu.id != 0) {
      glActiveTexture(textureId);
      glBindTexture(GL_TEXTURE_2D, texture->gpu.id);
    }
  }
  mShader->SetBool(name, guid != GUID::NONE());
}
} // namespace Ember
