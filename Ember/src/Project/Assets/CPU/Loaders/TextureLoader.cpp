#include "TextureLoader.h"

#include "Core/Logger.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Ember {
namespace TextureLoader {
std::optional<CPUAsset::TextureData>
FromFile(const std::filesystem::path &path) {
  if (!std::filesystem::exists(path)) return std::nullopt;

  stbi_set_flip_vertically_on_load(false);

  int width, height, channels;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);
  if (!data) return std::nullopt;

  auto texture = CPUAsset::TextureData();
  texture.width = width;
  texture.height = height;
  texture.channels = channels;

  texture.pixels.resize(width * height * channels);
  memcpy(texture.pixels.data(), data, texture.pixels.size());

  stbi_image_free(data);

  return texture;
}
} // namespace TextureLoader
} // namespace Ember
