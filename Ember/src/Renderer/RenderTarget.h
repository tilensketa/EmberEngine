#pragma once

#include <glad/glad.h>

namespace Ember {
class RenderTarget {
public:
  RenderTarget(uint32_t width, uint32_t height);
  ~RenderTarget();

  void Bind() const;
  void Unbind() const;
  void Resize(uint32_t width, uint32_t height);

  uint32_t GetWidth() const { return mWidth; }
  uint32_t GetHeight() const { return mHeight; }
  GLuint GetTextureId() const { return mTextureId; }

private:
  void recreateFramebuffer();
  void createColorAttachment();
  void createDepthStencilAttachment();
  void cleanup();

private:
  GLuint mFramebufferId;
  GLuint mTextureId;
  GLuint mDepthStencilId;
  uint32_t mWidth, mHeight;
};
} // namespace Ember
