#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

template <typename T> class VBO {
public:
  GLuint id = 0;

  VBO(const std::vector<T> &data) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(),
                 GL_STATIC_DRAW);
    Unbind();
  }

  void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); }
  void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }
  void Delete() { glDeleteBuffers(1, &id); }
};
