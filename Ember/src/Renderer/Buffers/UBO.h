#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class UBO {
public:
  GLuint id = 0;
  GLuint binding = 0;
  GLsizeiptr size = 0;

  UBO(GLsizeiptr bufferSize, GLuint bindingPoint)
      : binding(bindingPoint), size(bufferSize) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void Bind() const { glBindBuffer(GL_UNIFORM_BUFFER, id); }

  void Unbind() const { glBindBuffer(GL_UNIFORM_BUFFER, 0); }

  void SetSubData(GLintptr offset, GLsizeiptr dataSize,
                  const void *data) const {
    glBindBuffer(GL_UNIFORM_BUFFER, id);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, dataSize, data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  void Delete() {
    glDeleteBuffers(1, &id);
    id = 0;
  }
};
