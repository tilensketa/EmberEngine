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
  mFlatShader = std::make_unique<Shader>(SHADERS, "flat.vert", "flat.frag");
  mLightUBO = std::make_unique<UBO>(sizeof(LightBlockCPU), 0);
}

void Renderer::Render(const RenderContext &ctx) {
  if (!ctx.renderTarget || !mShader || !mFlatShader) return;
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

  if (!ctx.scene) {
    ctx.renderTarget->Unbind();
    return;
  }

  mShader->Activate();
  mShader->SetMat4("uCamMatrix", cameraMatrix);
  mShader->SetInt("uViewMode", static_cast<int>(ctx.camera.viewMode));
  mShader->SetVec3("uCameraPos", ctx.cameraTransform.position);
  mShader->SetFloat("uTime", Input::GetTime());
  collectLights(ctx);
  renderModels(ctx, mShader.get());

  mFlatShader->Activate();
  mFlatShader->SetMat4("uCamMatrix", cameraMatrix);
  mFlatShader->SetVec3("uFlatColor", glm::vec3(0, 0, 0));
  renderLights(ctx, mFlatShader.get());
  mFlatShader->SetVec3("uFlatColor", glm::vec3(0, 1, 1));
  renderColliders(ctx, mFlatShader.get());

  ctx.renderTarget->Unbind();
}
void Renderer::bindTexture(const GUID &guid, const GLenum &textureId,
                           const std::string &name) {
  // TODO fix null texture rendering
  mShader->SetBool(name, guid != GUID::NONE());
  if (guid == GUID::NONE()) return;

  auto texture = mAssetRegistry.GetAsset<Asset::Texture>(guid);
  if (!texture) return;
  if (texture->gpu.id != 0) {
    glActiveTexture(textureId);
    glBindTexture(GL_TEXTURE_2D, texture->gpu.id);
  }
}
void Renderer::collectLights(const RenderContext &ctx) {
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
    lightBlock.lights[i].props = glm::vec4(light.intensity, light.radius, 0, 0);
    i++;
  }
  mLightUBO->SetSubData(0, sizeof(LightBlockCPU), &lightBlock);
}
void Renderer::renderLights(const RenderContext &ctx, Shader *shader) {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  auto lightView = ctx.scene->View<Component::Transform, Component::Light>();
  for (auto entity : lightView) {
    auto &transform = ctx.scene->Get<Component::Transform>(entity);
    auto &light = ctx.scene->Get<Component::Light>(entity);
    if (ctx.camera.viewMode != Component::Camera::ViewMode::Debug) continue;

    AssetHandle handle = AssetHandle::NONE();
    if (light.lightType == Component::Light::LightType::Point) {
      handle = EngineAssetLoader::GetPrimitive(EnginePrimitive::Sphere);
    }
    // TODO visualisation for directional light
    if (handle == AssetHandle::NONE()) continue;
    auto model = mAssetRegistry.GetAsset<Asset::Model>(handle.guid);
    if (!model) continue;

    shader->SetMat4("uModel", transform.GetMatrix());
    for (const auto &mesh : model->meshes) {
      mesh.gpu.vao->Bind();
      glDrawElements(GL_TRIANGLES, mesh.gpu.indexCount, GL_UNSIGNED_INT, 0);
      mesh.gpu.vao->Unbind();
    }
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void Renderer::renderModels(const RenderContext &ctx, Shader *shader) {
  if (ctx.camera.viewMode == Component::Camera::ViewMode::Wireframe)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  auto modelView = ctx.scene->View<Component::Transform, Component::Model>();
  for (const auto entity : modelView) {
    const auto &transform = ctx.scene->Get<Component::Transform>(entity);
    const auto &model = ctx.scene->Get<Component::Model>(entity);
    if (!model.model) continue;
    shader->SetMat4("uModel", transform.GetMatrix());

    const GUID &guid = model.model->guid;
    auto modelAsset = mAssetRegistry.GetAsset<Asset::Model>(guid);
    if (!modelAsset) continue;

    for (size_t i = 0; i < modelAsset->meshes.size(); i++) {
      const auto &mesh = modelAsset->meshes[i];
      const GUID &meshMatGuid = mesh.material;
      const GUID &overrideMatGuid = model.materialOverrides[i];
      GUID matGuid =
          (overrideMatGuid == GUID::NONE()) ? meshMatGuid : overrideMatGuid;
      if (matGuid == GUID::NONE()) continue;
      const auto material = mAssetRegistry.GetAsset<Asset::Material>(matGuid);
      if (!material) continue;

      bindTexture(material->albedoMap, GL_TEXTURE0, "uHasAlbedoMap");
      bindTexture(material->normalMap, GL_TEXTURE1, "uHasNormalMap");
      bindTexture(material->metalnessMap, GL_TEXTURE2, "uHasMetalnessMap");
      bindTexture(material->roughnessMap, GL_TEXTURE3, "uHasRoughnessMap");

      shader->SetVec3("uAlbedoColor", material->factors.albedoColor);
      shader->SetFloat("uEmission", material->factors.emission);
      shader->SetFloat("uMetalness", material->factors.metalness);
      shader->SetFloat("uRoughness", material->factors.roughness);

      mesh.gpu.vao->Bind();
      glDrawElements(GL_TRIANGLES, mesh.gpu.indexCount, GL_UNSIGNED_INT, 0);
      mesh.gpu.vao->Unbind();
    }
  }
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
void Renderer::renderColliders(const RenderContext &ctx, Shader *shader) {
  if (ctx.camera.viewMode != Component::Camera::ViewMode::Debug) return;
  auto colliderView =
      ctx.scene->View<Component::Transform, Component::Collider>();
  glLineWidth(5.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  for (const auto entity : colliderView) {
    auto transform = ctx.scene->Get<Component::Transform>(entity);
    const auto &collider = ctx.scene->Get<Component::Collider>(entity);
    auto colliderTransform = Component::Transform();
    colliderTransform.position = collider.offset;
    colliderTransform.rotation = collider.rotation;

    AssetHandle handle = AssetHandle::NONE();
    if (collider.colliderType == Component::Collider::ColliderType::Box) {
      handle = EngineAssetLoader::GetPrimitive(EnginePrimitive::Cube);
      colliderTransform.scale = collider.boxScale;
    } else if (collider.colliderType ==
               Component::Collider::ColliderType::Sphere) {
      handle = EngineAssetLoader::GetPrimitive(EnginePrimitive::Sphere);
      colliderTransform.scale = glm::vec3(collider.radius);
      transform.scale = glm::vec3(glm::compMax(transform.scale));
    } else if (collider.colliderType ==
               Component::Collider::ColliderType::Capsule) {
      handle = EngineAssetLoader::GetPrimitive(EnginePrimitive::Capsule);
      colliderTransform.scale =
          glm::vec3(collider.radius, collider.radius, collider.height * 0.5f);
      float maxRadius = glm::max(transform.scale.x, transform.scale.y);
      transform.scale = glm::vec3(maxRadius, maxRadius, transform.scale.z);
    }
    auto model = mAssetRegistry.GetAsset<Asset::Model>(handle.guid);
    if (!model) continue;

    shader->SetMat4("uModel",
                    transform.GetMatrix() * colliderTransform.GetMatrix());

    for (const auto &mesh : model->meshes) {
      mesh.gpu.vao->Bind();
      glDrawElements(GL_TRIANGLES, mesh.gpu.indexCount, GL_UNSIGNED_INT, 0);
      mesh.gpu.vao->Unbind();
    }
  }
  glLineWidth(1.0f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
} // namespace Ember
