#pragma once

#include "AssetManager.h"

namespace Ember {
class ProjectAssetLoader {
public:
  static void Import(AssetManager &assetManager);
  static void Load(AssetManager &assetManager);
};
} // namespace Ember
