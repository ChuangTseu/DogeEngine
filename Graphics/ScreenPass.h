#ifndef SCREENPASS_H
#define SCREENPASS_H

#include "GL.h"

#include "shader.h"
#include "mesh.h"

#include <string>

class ScreenPass
{
    Shader m_shader;

    Mesh m_screenQuad;

    int m_width;
    int m_height;

public:
    ScreenPass(int width, int height, std::string shaderFilename) :
        m_width(width),
        m_height(height)
    {
        m_shader.addVertexShader("quadFbo.vert");
        m_shader.addFragmentShader(shaderFilename);
        m_shader.link();

        m_screenQuad.loadFullscreenQuad();
    }

    void resize(int width, int height) {
        m_width = width;
        m_height = height;
    }

    Shader& getShader() {
        return m_shader;
    }

    void fire() {
        GLint oldViewport[4];
        GL(glGetIntegerv(GL_VIEWPORT, oldViewport));

        m_shader.use();

        GL(glUniform1i(glGetUniformLocation(m_shader.getProgramId(), "screenWidth"), m_width));
        GL(glUniform1i(glGetUniformLocation(m_shader.getProgramId(), "screenHeight"), m_height));

        GL(glViewport(0, 0, m_width, m_height));

        m_screenQuad.draw();

        GL(glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]));
    }
};


class LEADR_screenpass
{
    Shader m_shader;

    Mesh m_screenQuad;

    int m_width;
    int m_height;

public:
    LEADR_screenpass(int width, int height) :
        m_width(width),
        m_height(height)
    {
        m_shader.addVertexShader("screen_leadr.vert");
        m_shader.addFragmentShader("screen_leadr.frag");
        m_shader.link();

        m_screenQuad.loadFullscreenQuad();
    }

    void resize(int width, int height) {
        m_width = width;
        m_height = height;
    }

    Shader& getShader() {
        return m_shader;
    }

    void fire() {
        GLint oldViewport[4];
        GL(glGetIntegerv(GL_VIEWPORT, oldViewport));

        m_shader.use();

        GL(glUniform1i(glGetUniformLocation(m_shader.getProgramId(), "screenWidth"), m_width));
        GL(glUniform1i(glGetUniformLocation(m_shader.getProgramId(), "screenHeight"), m_height));

        GL(glViewport(0, 0, m_width, m_height));

        m_screenQuad.draw();

        GL(glViewport(oldViewport[0], oldViewport[1], oldViewport[2], oldViewport[3]));
    }
};

#endif // SCREENPASS_H
