#pragma once

#include <imgui.h>

namespace Ember {
struct ImGuiFontRegistry {
  ImFont *pMain = nullptr;
  ImFont *pCodicons = nullptr;

  static ImGuiFontRegistry& Get() {
    static ImGuiFontRegistry instance;
    return instance;
  }
};
} // namespace Ember
