#pragma once

#include <glad/glad.h>
#include <vector>

class EBO {
public:
  GLuint id = 0;

  EBO(const std::vector<GLuint> &indices) {
    glGenBuffers(1, &id);
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
                 indices.data(), GL_STATIC_DRAW);
  }

  void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id); }
  void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); };
  void Delete() { glDeleteBuffers(1, &id); }
};
