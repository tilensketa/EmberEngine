#pragma once

#include "EditorPanels/IEditorPanel.h"

#include "EditorPanels/EditorHelpers.h"
#include "EditorState.h"
#include "Ember.h"

#include <imgui.h>

namespace Ember {
namespace Inspector {

#define NAME(T) Component::ComponentName<T>()

template <typename Fn> bool DrawComponentUI(const char *name, Fn fn) {
  bool open = true;
  if (ImGui::CollapsingHeader(name, &open, ImGuiTreeNodeFlags_DefaultOpen)) {
    ImGui::PushID(name);
    fn();
    ImGui::PopID();
  }
  return !open;
}

template <typename T> bool DrawComponent(T &c, EditorContext &context);

template <>
inline bool DrawComponent<Component::Id>(Component::Id &c,
                                         EditorContext &context) {
  ImGui::Text("%s: %lu", NAME(Component::Id), c.guid.GetID());
  return false;
}
template <>
inline bool DrawComponent<Component::Tag>(Component::Tag &c,
                                          EditorContext &context) {
  char buffer[32];
  strncpy(buffer, c.value.c_str(), sizeof(buffer));
  buffer[sizeof(buffer) - 1] = '\0';
  ImGui::InputText(NAME(Component::Tag), buffer, sizeof(buffer));
  c.value = buffer;
  return false;
}
template <>
inline bool DrawComponent<Component::Transform>(Component::Transform &c,
                                                EditorContext &context) {
  return DrawComponentUI(NAME(Component::Transform), [&]() {
    ImGui::DragFloat3("Position", &c.position.x, 0.1f);
    ImGui::DragFloat4("Rotation", &c.rotation.x, 0.1f, 0.0f, 1.0f);
    ImGui::DragFloat3("Scale", &c.scale.x, 0.1f, 0.0f, 100.0f);
  });
}
template <>
inline bool DrawComponent<Component::Model>(Component::Model &c,
                                            EditorContext &context) {
  return DrawComponentUI(NAME(Component::Model), [&]() {
    ImGui::Text("Model source:\t");
    ImGui::SameLine();
    std::string modelName = c.model ? c.model->path.string() : "None";

    if (ImGui::Selectable(modelName.c_str())) {
      ImGui::OpenPopup("ModelAssets");
    }
    auto selectedModelHandle = EditorHelpers::AssetSelector(
        context.assetRegistry, AssetType::MODEL, "ModelAssets", false);
    if (selectedModelHandle) {
      c.model = selectedModelHandle;
      auto modelAsset = context.assetRegistry.GetAsset<Asset::Model>(
          selectedModelHandle->guid);
      c.materialOverrides.clear();
      c.materialOverrides.assign(modelAsset->meshes.size(), GUID::NONE());
    }

    if (c.model) {
      auto model = context.assetRegistry.GetAsset<Asset::Model>(c.model->guid);
      int meshDisplayCount = glm::clamp(int(model->meshes.size()), 1, 5);
      ImGuiStyle &style = ImGui::GetStyle();
      float rowHeight = ImGui::GetFrameHeightWithSpacing();
      float childHeight = meshDisplayCount * rowHeight - style.ItemSpacing.y +
                          style.WindowPadding.y * 2 + style.ChildBorderSize * 2;
      ImGui::BeginChild("Materials", ImVec2(0, childHeight),
                        ImGuiChildFlags_Borders);
      for (size_t i = 0; i < model->meshes.size(); i++) {
        ImGui::PushID(i);
        auto &mesh = model->meshes[i];
        GUID meshMatGuid = mesh.material;
        GUID &overrideMatGuid = c.materialOverrides[i];
        GUID matGuid =
            (overrideMatGuid == GUID::NONE()) ? meshMatGuid : overrideMatGuid;

        auto matHandle = context.assetRegistry.GetAssetHandleByGuid(matGuid);
        std::string matName = (matHandle) ? matHandle->path.string() : "None";
        ImGui::Text("Mat_%zu", i);
        ImGui::SameLine();

        // Sizes for right-side buttons
        const float buttonVWidth =
            ImGui::CalcTextSize("V").x + ImGui::GetStyle().FramePadding.x * 2;
        const float buttonXWidth =
            ImGui::CalcTextSize("X").x + ImGui::GetStyle().FramePadding.x * 2;
        const float spacing = ImGui::GetStyle().ItemSpacing.x;

        float materialButtonWidth = ImGui::GetContentRegionAvail().x -
                                    buttonVWidth - buttonXWidth - spacing * 2;

        if (ImGui::Button(matName.c_str(), ImVec2(materialButtonWidth, 0))) {
          context.editorState.temporary.selectedMaterial = matGuid;
        }
        ImGui::SameLine();

        if (ImGui::Button("V")) ImGui::OpenPopup("MaterialAssets");
        ImGui::SameLine();
        ImGui::BeginDisabled(overrideMatGuid == GUID::NONE());
        if (ImGui::Button("X")) {
          overrideMatGuid = GUID::NONE();
        }
        ImGui::EndDisabled();

        auto selectedMatHandle = EditorHelpers::AssetSelector(
            context.assetRegistry, AssetType::MATERIAL, "MaterialAssets",
            false);
        if (selectedMatHandle) {
          overrideMatGuid = selectedMatHandle->guid;
          context.editorState.temporary.selectedMaterial =
              selectedMatHandle->guid;
        }
        ImGui::PopID();
      }
      ImGui::EndChild();
    }
  });
}
template <>
inline bool DrawComponent<Component::Camera>(Component::Camera &c,
                                             EditorContext &context) {
  return DrawComponentUI(NAME(Component::Camera), [&]() {
    auto projNames = Utils::EnumTraits<Component::Camera::Projection>::Names;
    int currentProj = static_cast<int>(c.projection);
    ImGui::Combo("Projection", &currentProj, projNames.data(),
                 static_cast<int>(projNames.size()));
    c.projection = static_cast<Component::Camera::Projection>(currentProj);

    auto viewNames = Utils::EnumTraits<Component::Camera::ViewMode>::Names;
    int currentMode = static_cast<int>(c.viewMode);
    ImGui::Combo("ViewMode", &currentMode, viewNames.data(),
                 static_cast<int>(viewNames.size()));
    c.viewMode = static_cast<Component::Camera::ViewMode>(currentMode);

    ImGui::DragScalar("Resolution X", ImGuiDataType_U32, &c.resolution.x);
    c.resolution.x = glm::max(c.resolution.x, uint32_t(1));
    ImGui::DragScalar("Resolution Y", ImGuiDataType_U32, &c.resolution.y);
    c.resolution.y = glm::max(c.resolution.y, uint32_t(1));
    ImGui::Text("Aspect: %f", c.aspect);
    ImGui::DragFloat("Near", &c.near);
    ImGui::DragFloat("Far", &c.far);
    if (c.projection == Component::Camera::Projection::Perspective) {
      ImGui::DragFloat("Fov", &c.fov);
    } else if (c.projection == Component::Camera::Projection::Orthographic) {
      ImGui::DragFloat("Ortho", &c.orthoSize, 0.1f);
      c.orthoSize = glm::max(0.1f, c.orthoSize);
    }
    ImGui::Checkbox("Active", &c.active);
  });
}
template <>
inline bool DrawComponent<Component::Light>(Component::Light &c,
                                            EditorContext &context) {
  return DrawComponentUI(NAME(Component::Light), [&]() {
    auto lightNames = Utils::EnumTraits<Component::Light::LightType>::Names;

    int lightType = static_cast<int>(c.lightType);
    ImGui::Combo("LightType", &lightType, lightNames.data(),
                 static_cast<int>(lightNames.size()));
    c.lightType = static_cast<Component::Light::LightType>(lightType);

    ImGui::ColorEdit3("Color", &c.color.r);
    ImGui::DragFloat("Intensity", &c.intensity, 0.1f, 0.0f, 100.0f);
    ImGui::DragFloat("Radius", &c.radius, 0.1f, 0.1f, 100.0f);
  });
}
template <>
inline bool DrawComponent<Component::Rigidbody>(Component::Rigidbody &c,
                                                EditorContext &context) {
  return DrawComponentUI(NAME(Component::Rigidbody), [&]() {
    auto bodyNames = Utils::EnumTraits<Component::Rigidbody::BodyType>::Names;

    int bodyType = static_cast<int>(c.bodyType);
    ImGui::Combo("BodyType", &bodyType, bodyNames.data(),
                 static_cast<int>(bodyNames.size()));
    c.bodyType = static_cast<Component::Rigidbody::BodyType>(bodyType);

    ImGui::DragFloat("Mass", &c.mass, 0.1f, 0.0f, 100.0f);
    ImGui::Checkbox("UseGravity", &c.useGravity);
  });
}
template <>
inline bool DrawComponent<Component::Collider>(Component::Collider &c,
                                               EditorContext &context) {
  return DrawComponentUI(NAME(Component::Collider), [&]() {
    auto colliderNames =
        Utils::EnumTraits<Component::Collider::ColliderType>::Names;

    int colliderType = static_cast<int>(c.colliderType);
    ImGui::Combo("ColliderType", &colliderType, colliderNames.data(),
                 static_cast<int>(colliderNames.size()));
    c.colliderType =
        static_cast<Component::Collider::ColliderType>(colliderType);

    ImGui::DragFloat3("Offset", &c.offset[0], 0.1f, -10.0f, 10.0f);
    ImGui::DragFloat4("Rotation", &c.rotation[0], 0.1f, 0.0f, 1.0f);

    if (c.colliderType == Component::Collider::ColliderType::Box) {
      ImGui::DragFloat3("BoxScale", &c.boxScale[0], 0.1f, 0.0f, 10.0f);
    } else if (c.colliderType == Component::Collider::ColliderType::Sphere) {
      ImGui::DragFloat("Radius", &c.radius);
    } else if (c.colliderType == Component::Collider::ColliderType::Capsule) {
      ImGui::DragFloat("Radius", &c.radius);
      ImGui::DragFloat("Height", &c.height);
    }
    ImGui::Checkbox("IsTrigger", &c.isTrigger);
  });
}
} // namespace Inspector
} // namespace Ember
