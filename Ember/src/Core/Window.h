#pragma once

#include "Core/Events/IEvent.h"
#include <functional>
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

namespace Ember {
struct WindowSpecification {
  std::string title = "EMBER";
  uint32_t width = 1280;
  uint32_t height = 720;
  bool useVSync = true;
};

class Window {
public:
  Window(const WindowSpecification &specification = WindowSpecification());
  ~Window();

  void UpdateBegin();
  void UpdateEnd();
  inline bool ShouldClose() { return glfwWindowShouldClose(mWindow); }

  GLFWwindow *GetRawPtr() { return mWindow; }

  using EventCallbackFn = std::function<void(Event::IEvent &)>;
  void SetEventCallback(const EventCallbackFn &callback) {
    mEventCallback = callback;
  }

  uint32_t GetWidth() const {return mWidth;}
  uint32_t GetHeight() const {return mHeight;}
  void SetWidth(uint32_t width) {mWidth = width; }
  void SetHeight(uint32_t height) {mHeight = height;}

public:
  EventCallbackFn mEventCallback;
private:
  GLFWwindow *mWindow = nullptr;
  uint32_t mWidth;
  uint32_t mHeight;
};
} // namespace Ember
