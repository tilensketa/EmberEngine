#include "Window.h"

#include "Events/Events.h"
#include "Input.h"
#include "Logger.h"

namespace Ember {
static void framebufferSizeCallback(GLFWwindow *glfwWindow, int width, int height) {
  Window* window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
  if (!window) return;

  Event::WindowResize e(width, height);
  window->mEventCallback(e);

  window->SetWidth(width);
  window->SetHeight(height);
  glViewport(0,0,width, height);
}
static void KeyCallback(GLFWwindow *glfwWindow, int key, int scancode,
                        int action, int mods) {
  Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
  if (window && !window->mEventCallback) return;

  if (action == GLFW_PRESS) {
    Event::KeyPress e(key);
    window->mEventCallback(e);
    Input::OnKeyEvent(key, true);
  } else if (action == GLFW_RELEASE) {
    Event::KeyRelease e(key);
    window->mEventCallback(e);
    Input::OnKeyEvent(key, false);
  } else if (action == GLFW_REPEAT) {
    Event::KeyRepeat e(key);
    window->mEventCallback(e);
  }
}
static void MouseButtonCallback(GLFWwindow *glfwWindow, int button, int action,
                                int mods) {
  Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
  if (window && !window->mEventCallback) return;

  if (action == GLFW_PRESS) {
    Event::MouseButtonPress e(button);
    window->mEventCallback(e);
    Input::OnMouseButtonEvent(button, true);
  } else if (action == GLFW_RELEASE) {
    Event::MouseButtonRelease e(button);
    window->mEventCallback(e);
    Input::OnMouseButtonEvent(button, false);
  }
}
static void MouseMoveCallback(GLFWwindow *glfwWindow, double xpos,
                              double ypos) {
  Window *window = static_cast<Window *>(glfwGetWindowUserPointer(glfwWindow));
  if (window && !window->mEventCallback) return;

  Event::MouseMove e(xpos, ypos);
  window->mEventCallback(e);
  Input::OnMouseMoveEvent(xpos, ypos);
}

Window::Window(const WindowSpecification &specification) {
  LOG_INFO("Initializing window...");
  if (!glfwInit()) {
    LOG_CRITICAL("Failed to initialize glfw");
  } else {
    LOG_INFO("Successfully initialized glfw");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  mWidth = specification.width;
  mHeight = specification.height;
  mWindow = glfwCreateWindow(mWidth, mHeight, specification.title.c_str(),
                             nullptr, nullptr);
  if (!mWindow) {
    LOG_CRITICAL("Failed to create window");
  } else {
    LOG_INFO("Successfully created window");
  }

  glfwMakeContextCurrent(mWindow);
  glfwSetWindowUserPointer(mWindow, this);

  glfwSetFramebufferSizeCallback(mWindow, framebufferSizeCallback);
  glfwSetKeyCallback(mWindow, KeyCallback);
  glfwSetMouseButtonCallback(mWindow, MouseButtonCallback);
  glfwSetCursorPosCallback(mWindow, MouseMoveCallback);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    LOG_CRITICAL("Failed to initialize glad");
  } else {
    LOG_INFO("Successfully initialized glad");
  }

  glViewport(0, 0, specification.width, specification.height);
  glfwSwapInterval(specification.useVSync);
  LOG_INFO("Window initialized");

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_FRAMEBUFFER_SRGB); // TODO fix srgb
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glDisable(GL_CULL_FACE);
};
Window::~Window() {
  glfwDestroyWindow(mWindow);
  glfwTerminate();
}
void Window::UpdateBegin() {
  glClearColor(0, 1, 1, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glfwPollEvents();
};
void Window::UpdateEnd() { glfwSwapBuffers(mWindow); };
} // namespace Ember
