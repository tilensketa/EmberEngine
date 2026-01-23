#include "Renderer/Shader.h"

#include "Core/Logger.h"

#include "glm/gtc/type_ptr.hpp"
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>

Shader::Shader(const char *vertexShaderPath, const char *fragmentShaderPath) {
  std::string vertexShaderCode = readShaderFile(vertexShaderPath);
  const char *vertexShaderSource = vertexShaderCode.c_str();
  std::string fragmentShaderCode = readShaderFile(fragmentShaderPath);
  const char *fragmentShaderSource = fragmentShaderCode.c_str();

  GLuint vertexShader =
      compileShader(GL_VERTEX_SHADER, vertexShaderSource, vertexShaderPath);
  GLuint fragmentShader = compileShader(
      GL_FRAGMENT_SHADER, fragmentShaderSource, fragmentShaderPath);

  // Link Shaders into Shader Program
  mID = glCreateProgram();
  glAttachShader(mID, vertexShader);
  glAttachShader(mID, fragmentShader);
  glLinkProgram(mID);

  // Check for linking errors
  GLint successLink;
  glGetProgramiv(mID, GL_LINK_STATUS, &successLink);
  if (!successLink) {
    GLchar infoLog[1024];
    glGetProgramInfoLog(mID, 512, NULL, infoLog);
    std::ostringstream errorMsg;
    LOG_CRITICAL("Shader Program Linking Failed:\n {}", infoLog);
  }

  if (successLink) {
    LOG_INFO("Shader compiled: " + std::string(vertexShaderPath) + ", " +
             std::string(fragmentShaderPath));
  } else {
    LOG_CRITICAL("Shader failed to compile: " + std::string(vertexShaderPath) +
                 ", " + std::string(fragmentShaderPath));
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

Shader::Shader(const std::string &folder, const std::string &vertex,
               const std::string &fragment)
    : Shader((folder + vertex).c_str(), (folder + fragment).c_str()) {
  LOG_INFO("Shader: Making shader");
}

std::string Shader::readShaderFile(const char *filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    LOG_ERROR("Failed to open file: " + std::string(filePath));
    return "";
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}

GLuint Shader::compileShader(GLenum type, const char *source,
                             const std::string &name) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    GLchar infoLog[1024];
    glGetShaderInfoLog(shader, 1024, NULL, infoLog);
    LOG_CRITICAL("Shader compilation failed (" + name + "): " + infoLog);
  }
  return shader;
}

void Shader::Activate() { glUseProgram(mID); }
void Shader::Delete() { glDeleteProgram(mID); }

void Shader::SetBool(const std::string &name, bool value) const {
  glUniform1i(getLocation(name.c_str()), (int)value);
}
void Shader::SetInt(const std::string &name, int value) const {
  glUniform1i(getLocation(name.c_str()), value);
}
void Shader::SetFloat(const std::string &name, const float value) const {
  glUniform1f(getLocation(name.c_str()), value);
}
void Shader::SetVec3(const std::string &name, const glm::vec3 &value) const {
  glUniform3f(getLocation(name.c_str()), value.x, value.y, value.z);
}
void Shader::SetVec4(const std::string &name, const glm::vec4 &value) const {
  glUniform4f(getLocation(name.c_str()), value.x, value.y, value.z, value.w);
}
void Shader::SetMat4(const std::string &name, const glm::mat4 &value) const {
  glUniformMatrix4fv(getLocation(name.c_str()), 1, GL_FALSE,
                     glm::value_ptr(value));
}

int Shader::getLocation(const std::string &name) const {
  int location = glGetUniformLocation(mID, name.c_str());
  return location;
}
