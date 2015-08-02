#include "vao.h"

VAO::VAO()
{
    GL(glGenVertexArrays(1, &m_vaoId));
}

VAO::~VAO()
{
    (glDeleteVertexArrays(1, &m_vaoId));
}

void VAO::bind() const
{
    GL(glBindVertexArray(m_vaoId));
}

void VAO::unbind() {
    GL(glBindVertexArray(0));
}
