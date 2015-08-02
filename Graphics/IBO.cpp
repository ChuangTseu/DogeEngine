#include "ibo.h"

IBO::IBO()
{
    GL(glGenBuffers(1, &m_iboId));
}

IBO::~IBO()
{
    (glDeleteBuffers(1, &m_iboId));
}

void IBO::submitData(GLuint* indices_data, size_t size) {
    bind();

    GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, size*sizeof(GLuint), nullptr, GL_STATIC_DRAW));
    GL(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size*sizeof(GLuint), indices_data));

    IBO::unbind();
}

void IBO::bind() {
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboId));
}

void IBO::unbind() {
    GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
