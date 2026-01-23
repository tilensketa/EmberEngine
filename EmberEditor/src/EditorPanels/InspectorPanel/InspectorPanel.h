#pragma once

#include "EditorPanels/IEditorPanel.h"
#include "Inspector.h"

namespace Ember {
class InspectorPanel : public IEditorPanel {
public:
  InspectorPanel(EditorContext &ctx) : IEditorPanel(ctx) {}

  virtual void Update() override;
  virtual void OnEvent(Ember::Event::IEvent &event) override;

private:
  template <typename Tuple, std::size_t Index = 0>
  void drawComponent(Scene *scene, EntityHandle handle) {
    if constexpr (Index < std::tuple_size_v<Tuple>) {
      using ComponentType = std::tuple_element_t<Index, Tuple>;
      std::string name = Component::ComponentName<ComponentType>();

      if (scene->HasComponent<ComponentType>(handle)) {
        ComponentType &component = scene->Get<ComponentType>(handle);
        if (Inspector::DrawComponent(component, mCtx))
          scene->RemoveComponent<ComponentType>(handle);
      }

      drawComponent<Tuple, Index + 1>(scene, handle);
    }
  }

  template <typename Tuple, std::size_t Index = 0>
  void showAddComponentDropdown(Scene *scene, EntityHandle handle,
                                const char *filter) {
    if constexpr (Index < std::tuple_size_v<Tuple>) {
      using ComponentType = std::tuple_element_t<Index, Tuple>;
      std::string name = Component::ComponentName<ComponentType>();
      std::string nameLower = name;
      std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      std::string str(filter);
      std::transform(str.begin(), str.end(), str.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      auto it = nameLower.find(str);
      if (it != nameLower.npos || str == "") {
        if (!scene->HasComponent<ComponentType>(handle)) {
          if (ImGui::Selectable(name.c_str())) {
            scene->AddComponent<ComponentType>(handle);
            mShowAddComponent = false;
          }
        }
      }

      showAddComponentDropdown<Tuple, Index + 1>(scene, handle, filter);
    }
  }

private:
  bool mShowAddComponent = false;
};
} // namespace Ember
