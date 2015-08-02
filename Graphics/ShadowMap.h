#ifndef SHADOWMAP_H
#define SHADOWMAP_H

#include "GL.h"
#include "fbo.h"
#include "MathsTools/mat4.h"

class Shadowmap
{
    FBO m_shadowFbo;

    GLint m_oldViewport[4];

public:
    Shadowmap(size_t width, size_t height) : m_shadowFbo(width, height, 0, true) {

    }

    void bind() {
        GL(glGetIntegerv(GL_VIEWPORT, m_oldViewport));

        GL(glViewport(0, 0, m_shadowFbo.getWidth(), m_shadowFbo.getHeight()));

        m_shadowFbo.bind();

        GL(glClear(GL_DEPTH_BUFFER_BIT));

        GL(glDrawBuffer(GL_NONE));
        GL(glReadBuffer(GL_NONE));
    }

    void unbind() {
        FBO::unbind();

        //Restore old viewport
        GL(glViewport(m_oldViewport[0], m_oldViewport[1], m_oldViewport[2], m_oldViewport[3]));
    }

    void bindShadowMapToTarget(GLuint target) {
        m_shadowFbo.getTexture(0).bindToTarget(target);
    }
};

#endif // SHADOWMAP_H
