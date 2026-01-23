#pragma once

#include "GLFW/glfw3.h"
#include <unordered_map>

namespace Ember {
class Input {
public:
  static bool IsKeyDown(int key) {
    auto it = sKeyStates.find(key);
    return it != sKeyStates.end() && it->second;
  }
  static bool IsMouseButtonDown(int key) {
    auto it = sMouseButtonStates.find(key);
    return it != sMouseButtonStates.end() && it->second;
  }
  static float GetTime() { return glfwGetTime(); }
  static double GetMouseX() { return sMouseX; }
  static double GetMouseY() { return sMouseY; }

  static void OnKeyEvent(int key, bool pressed) { sKeyStates[key] = pressed; }
  static void OnMouseButtonEvent(int key, bool pressed) {
    sMouseButtonStates[key] = pressed;
  }
  static void OnMouseMoveEvent(double x, double y) { sMouseX = x, sMouseY = y; }

private:
  static std::unordered_map<int, bool> sKeyStates;
  static std::unordered_map<int, bool> sMouseButtonStates;
  static double sMouseX, sMouseY;
};
} // namespace Ember
