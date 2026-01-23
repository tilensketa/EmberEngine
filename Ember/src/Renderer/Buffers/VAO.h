#pragma once

#include "Renderer/Buffers/VBO.h"
#include <glad/glad.h>

class VAO {
public:
  GLuint id = 0;

  VAO() { glGenVertexArrays(1, &id); }

  template <typename T>
  void LinkAttrib(VBO<T> &Vbo, GLuint layout, GLuint numComponents, GLenum type,
                  GLsizeiptr stride, void *offset) {
    Vbo.Bind();
    glVertexAttribPointer(layout, numComponents, type, GL_FALSE,
                          static_cast<GLsizei>(stride), offset);
    glEnableVertexAttribArray(layout);
    Vbo.Unbind();
  }
  void Bind() const { glBindVertexArray(id); }
  void Unbind() const { glBindVertexArray(0); }
  void Delete() { glDeleteVertexArrays(1, &id); }
};
