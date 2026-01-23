#pragma once

#include "Ember.h"

#include <imgui.h>
#include <optional>
#include <string>
#include <vector>

namespace Ember {
namespace EditorHelpers {

inline std::optional<AssetHandle>
AssetSelector(AssetRegistry& assetRegistry,
              const AssetType &type, const std::string &popupName,
              bool canSelectNone,
              std::optional<AssetDomain> filterDomain = std::nullopt) {
  std::optional<AssetHandle> result;

  if (!ImGui::BeginPopup(popupName.c_str())) return result;

  ImGui::SeparatorText(popupName.c_str());

  // Search buffer
  static char searchBuffer[128] = {};
  ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::InputText("###Search", searchBuffer, sizeof(searchBuffer));
  std::string filter(searchBuffer);

  // Build list of handles from registry
  std::vector<AssetHandle> handles;

  // Iterate domains in registry (priority order)
  for (auto domain :
       {AssetDomain::Project, AssetDomain::Engine}) {
    if ((filterDomain && *filterDomain != domain) && filterDomain) continue;

    auto manager = assetRegistry.GetAssetManagerByDomain(domain);
    if (!manager) continue;

    auto domainHandles = manager->GetAssetHandlesByType(type);
    handles.insert(handles.end(), domainHandles.begin(), domainHandles.end());
  }

  // Option to select "none"
  if (canSelectNone) {
    if (ImGui::Selectable("NONE")) {
      result = AssetHandle::NONE();
      ImGui::CloseCurrentPopup();
      ImGui::EndPopup();
      return result;
    }
  }

  // Display selectable items
  for (const auto &handle : handles) {
    std::string handleString = handle.path.string() +
                               std::to_string(handle.guid.GetID()) +
                               Utils::ToString<AssetType>(handle.type);

    if (filter.empty() || handleString.find(filter) != std::string::npos) {
      if (ImGui::Selectable(handle.path.filename().string().c_str())) {
        result = handle;
        ImGui::CloseCurrentPopup();
        break;
      }
    }
  }

  ImGui::EndPopup();
  return result;
}

} // namespace EditorHelpers
} // namespace Ember
