#ifndef ORTHOBASE_H
#define ORTHOBASE_H

#include <vector>
#include <iostream>

#include <assimp/postprocess.h>

#include "GL.h"

#include "vbo.h"
#include "ibo.h"

#include "vertex.h"
#include "model.h"

#include "shader.h"

class OrthoBase
{
    VBO m_vbo;
    GLuint m_vao;

    std::vector<Vertex> m_vertices;

    Shader m_lineShader;

    Model m_arrow;

public:
    OrthoBase() {
        vec3 origin{0, 0, 0};
        vec3 xaxis{1, 0, 0};
        vec3 yaxis{0, 1, 0};
        vec3 zaxis{0, 0, 1};

        vec2 uvcoord{0, 0};
        vec3 tangent{0, 0, 0};

        float axisLength = 10.f;

        m_vertices = std::vector<Vertex>{
                Vertex{origin, xaxis, uvcoord, tangent}, Vertex{xaxis*axisLength, xaxis, uvcoord, tangent},
                Vertex{origin, yaxis, uvcoord, tangent}, Vertex{yaxis*axisLength, yaxis, uvcoord, tangent},
                Vertex{origin, zaxis, uvcoord, tangent}, Vertex{zaxis*axisLength, zaxis, uvcoord, tangent}
                                        };

        m_vbo.submitData(m_vertices.data(), m_vertices.size());

        GL(glGenVertexArrays(1, &m_vao));
        GL(glBindVertexArray(m_vao));

        m_vbo.bind();
        GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), 0));
        GL(glEnableVertexAttribArray(0));

        GL(glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), BUFFER_OFFSET(sizeof(vec3))));
        GL(glEnableVertexAttribArray(1));


        GL(glBindVertexArray(0));

        VBO::unbind();


        m_lineShader.addVertexShader("line.vert");
        m_lineShader.addFragmentShader("line.frag");
        m_lineShader.link();

        m_arrow.loadFromFile("arrow_cone.obj");
    }

    void draw(const mat4& projection, const mat4& view) {
        vec3 red{1.f, 0.f, 0.f};
        vec3 green{0.f, 1.f, 0.f};
        vec3 blue{0.f, 0.f, 1.f};

        mat4 arrowTransformation;

        m_lineShader.use();
        m_lineShader.sendTransformations(projection, view, mat4::Identity());
//        GL(glUniform1i(glGetUniformLocation(m_lineShader.getProgramId(), "overrideColor"), false));
        GL(glUniform1i(glGetUniformLocation(m_lineShader.getProgramId(), "overrideColor"), false));
//        GL(glUniform3fv(glGetUniformLocation(m_lineShader.getProgramId(), "userColor"), 1, red.data()));

        GL(glBindVertexArray(m_vao));

        GL(glDrawArrays(GL_LINES, 0, 6));

        GL(glBindVertexArray(0));


        arrowTransformation = mat4::Identity();
        arrowTransformation.translate(10,0,0);
        arrowTransformation.rotate(normalize({0.f, 0.f, 1.f}), -90.f);

        m_lineShader.sendTransformations(projection, view, arrowTransformation);
        GL(glUniform1i(glGetUniformLocation(m_lineShader.getProgramId(), "overrideColor"), true));
        GL(glUniform3fv(glGetUniformLocation(m_lineShader.getProgramId(), "userColor"), 1, red.data()));

        m_arrow.drawAsTriangles();


        arrowTransformation = mat4::Identity();
        arrowTransformation.translate(0,10,0);

        m_lineShader.sendTransformations(projection, view, arrowTransformation);
        GL(glUniform1i(glGetUniformLocation(m_lineShader.getProgramId(), "overrideColor"), true));
        GL(glUniform3fv(glGetUniformLocation(m_lineShader.getProgramId(), "userColor"), 1, green.data()));

        m_arrow.drawAsTriangles();


        arrowTransformation = mat4::Identity();
        arrowTransformation.translate(0,0,10);
        arrowTransformation.rotate(normalize({1.f, 0.f, 0.f}), 90.f);

        m_lineShader.sendTransformations(projection, view, arrowTransformation);
        GL(glUniform1i(glGetUniformLocation(m_lineShader.getProgramId(), "overrideColor"), true));
        GL(glUniform3fv(glGetUniformLocation(m_lineShader.getProgramId(), "userColor"), 1, blue.data()));

        m_arrow.drawAsTriangles();

        GL(glUseProgram(0));
    }
};

#endif // ORTHOBASE_H
