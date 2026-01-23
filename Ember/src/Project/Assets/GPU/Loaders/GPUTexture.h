#pragma once

#include "../GPUAssets.h"
#include "Project/Assets/CPU/Loaders/TextureLoader.h"

namespace Ember {
namespace GPUTexture {

inline GLenum
getInternatFormat(const TextureLoader::ImportSettings::TextureType &textureType,
                  int channels) {
  switch (textureType) {
  case TextureLoader::ImportSettings::TextureType::Albedo:
    return channels == 4 ? GL_SRGB8_ALPHA8 : GL_SRGB8;
  case TextureLoader::ImportSettings::TextureType::Normal: return GL_RGB8;
  case TextureLoader::ImportSettings::TextureType::Metalness: return GL_R8;
  case TextureLoader::ImportSettings::TextureType::Roughness: return GL_R8;
  case TextureLoader::ImportSettings::TextureType::UI: return GL_SRGB8_ALPHA8;
  default: return GL_RGBA8;
  }
}
inline GLenum getDataFormat(int channels) {
  switch (channels) {
  case 4: return GL_RGBA;
  case 3: return GL_RGB;
  case 1: return GL_RED;
  default: return GL_RGBA;
  }
}
inline GLenum
getFilterType(const TextureLoader::ImportSettings::FilterType &filterType) {
  switch (filterType) {
  case TextureLoader::ImportSettings::FilterType::Nearest: return GL_NEAREST;
  case TextureLoader::ImportSettings::FilterType::Linear: return GL_LINEAR;
  default: return GL_LINEAR;
  }
}
inline GLenum
getWrapType(const TextureLoader::ImportSettings::WrapType &wrapType) {
  switch (wrapType) {
  case TextureLoader::ImportSettings::WrapType::ClampToEdge:
    return GL_CLAMP_TO_EDGE;
  case TextureLoader::ImportSettings::WrapType::ClampToBorder:
    return GL_CLAMP_TO_BORDER;
  case TextureLoader::ImportSettings::WrapType::MirrorClampToEdge:
    return GL_MIRROR_CLAMP_TO_EDGE;
  case TextureLoader::ImportSettings::WrapType::MirroredRepeat:
    return GL_MIRRORED_REPEAT;
  case TextureLoader::ImportSettings::WrapType::Repeat: return GL_REPEAT;
  default: return GL_REPEAT;
  }
}
inline GPUAsset::GPUTexture
FromTextureData(const CPUAsset::TextureData &textureData,
                const TextureLoader::ImportSettings &importSettings) {
  GLuint id;
  glGenTextures(1, &id);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, id);

  // linear for smooth blending, near for not
  GLenum filter = getFilterType(importSettings.filterType);
  GLenum wrap = getWrapType(importSettings.wrapType);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);

  GLenum internalFormat =
      getInternatFormat(importSettings.textureType, textureData.channels);
  GLenum format = getDataFormat(textureData.channels);
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, textureData.width,
               textureData.height, 0, format, GL_UNSIGNED_BYTE,
               textureData.pixels.data());
  if (importSettings.generateMipmaps) glGenerateMipmap(GL_TEXTURE_2D);

  return GPUAsset::GPUTexture{id};
}
} // namespace GPUTexture
} // namespace Ember
