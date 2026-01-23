#include "RenderTarget.h"

namespace Ember {
RenderTarget::RenderTarget(uint32_t width, uint32_t height)
    : mWidth(width), mHeight(height) {
  glGenFramebuffers(1, &mFramebufferId);
  recreateFramebuffer();
}
RenderTarget::~RenderTarget() { cleanup(); }

void RenderTarget::Bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, mFramebufferId);
}
void RenderTarget::Unbind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
void RenderTarget::Resize(uint32_t width, uint32_t height) {
  mWidth = width;
  mHeight = height;
  recreateFramebuffer();
}

void RenderTarget::recreateFramebuffer() {
  if (mTextureId) {
    glDeleteTextures(1, &mTextureId);
    mTextureId = 0;
  }
  if (mDepthStencilId) {
    glDeleteRenderbuffers(1, &mDepthStencilId);
    mDepthStencilId = 0;
  }

  Bind();
  createColorAttachment();
  createDepthStencilAttachment();
  Unbind();
}
void RenderTarget::createColorAttachment() {
  glGenTextures(1, &mTextureId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTextureId);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         mTextureId, 0);
}
void RenderTarget::createDepthStencilAttachment() {
  glGenRenderbuffers(1, &mDepthStencilId);
  glBindRenderbuffer(GL_RENDERBUFFER, mDepthStencilId);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, mWidth, mHeight);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, mDepthStencilId);
}
void RenderTarget::cleanup() {
  if (mTextureId) {
    glDeleteTextures(1, &mTextureId);
    mTextureId = 0;
  }
  if (mDepthStencilId) {
    glDeleteRenderbuffers(1, &mDepthStencilId);
    mDepthStencilId = 0;
  }
  if (mFramebufferId) {
    glDeleteFramebuffers(1, &mFramebufferId);
    mFramebufferId = 0;
  }
}
} // namespace Ember
