#pragma once

#include <Ember.h>

#include <imgui.h>

#include <ImGuizmo.h>

namespace Ember {
struct EditorState {
  struct {
    // Selected
    std::optional<EntityHandle> selectedEntityHandle = std::nullopt;
    std::optional<GUID> selectedMaterial = std::nullopt;
    // TODO fix file path
    std::filesystem::path selectedFolder;
    // Editor Contols
    ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE currentMode = ImGuizmo::WORLD;
    bool simulationActive = false;
    // Editor camera
    Component::Transform camTransform;
    Component::Camera camCamera;
    float camSpeed = 0.1f;
  } temporary;
  struct {

  } persistent;
};
} // namespace Ember
