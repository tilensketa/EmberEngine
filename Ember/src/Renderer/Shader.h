#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <string>

class Shader {
public:
  Shader(const char *vertexShaderPath, const char *fragmentShaderPath);
  Shader(const std::string &folder, const std::string &vertex,
         const std::string &fragment);
  GLuint compileShader(GLenum type, const char *source,
                       const std::string &name);

  void Activate();
  void Delete();

  const unsigned int GetID() const { return mID; }

  void SetBool(const std::string &name, bool value) const;
  void SetInt(const std::string &name, const int value) const;
  void SetFloat(const std::string &name, const float value) const;
  void SetVec3(const std::string &name, const glm::vec3 &value) const;
  void SetVec4(const std::string &name, const glm::vec4 &value) const;
  void SetMat4(const std::string &name, const glm::mat4 &value) const;
  int getLocation(const std::string &name) const;

private:
  std::string readShaderFile(const char *filePath);

private:
  unsigned int mID;
};
