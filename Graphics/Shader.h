#ifndef SHADER_H
#define SHADER_H

#include <algorithm>
#include <iostream>

#include "GL.h"

#include "MathsTools/mat4.h"

#include "material.h"

#include <string>
#include <memory>

enum EShaderStage { 
	VERTEX_SHADER = 0, 
	TESS_CONTROL_SHADER, 
	TESS_EVAL_SHADER, 
	GEOMETRY_SHADER, 
	FRAGMENT_SHADER, 
	
	NUM_SHADERS_STAGES };

static uint32_t LUT_EShaderStage_To_GLEnum[NUM_SHADERS_STAGES] = { 
	GL_VERTEX_SHADER, 
	GL_TESS_CONTROL_SHADER,
	GL_TESS_EVALUATION_SHADER, 
	GL_GEOMETRY_SHADER, 
	GL_FRAGMENT_SHADER 
};

struct ShaderDesc
{
	bool bStageEnable[NUM_SHADERS_STAGES];
	std::string stagesFilenames[NUM_SHADERS_STAGES];
};


class Shader {
public:
	Shader() {
        m_program = GL(glCreateProgram());
    }

    ~Shader() {
		GL_flushError();
        (glDeleteProgram(m_program)); // BOB_TODO : Investigate WHYYYYYY it crashes there
    }

	void swap(Shader& other)
	{
		using std::swap;

		swap(m_currentShaderFilenames, other.m_currentShaderFilenames);
		swap(m_program, other.m_program);
	}

	Shader(Shader&& other) : m_program(0)
	{
		swap(other);
	}

	void operator=(Shader&& other)
	{
		swap(other);
	}

	Shader(const Shader& other) = delete;
	void operator=(const Shader& other) = delete;

    static void unbind() {
        GL(glUseProgram(0));
    }

	void LoadFromDesc(const ShaderDesc& desc)
	{
		for (int i = 0; i < NUM_SHADERS_STAGES; ++i)
		{
			if (desc.bStageEnable[i])
			{
				addShaderStage(static_cast<EShaderStage>(i), desc.stagesFilenames[i]);
			}
		}

		link();
	}

	bool addShaderStage(EShaderStage eShaderStage, std::string filename)
	{
		m_currentShaderFilenames[eShaderStage] = filename;
		return addShaderStageInternal(filename, LUT_EShaderStage_To_GLEnum[eShaderStage]);
	}

    bool addVertexShader(std::string filename);
    bool addFragmentShader(std::string filename);
    bool addTessControlShader(std::string filename);
    bool addTessEvaluationShader(std::string filename);
    bool addGeometryShader(std::string filename);

    bool link();

    void use() const;

    GLuint getProgramId() const {
        return m_program;
    }

    void sendTransformations(const mat4 &projection, const mat4 &view, const mat4 &model);

    void sendMaterial(const Material& mat);

    void reload() {
        renew();

        if (!m_currentShaderFilenames[VERTEX_SHADER].empty()) addVertexShader(m_currentShaderFilenames[VERTEX_SHADER]);
		if (!m_currentShaderFilenames[TESS_CONTROL_SHADER].empty()) addTessControlShader(m_currentShaderFilenames[TESS_CONTROL_SHADER]);
		if (!m_currentShaderFilenames[TESS_EVAL_SHADER].empty()) addTessEvaluationShader(m_currentShaderFilenames[TESS_EVAL_SHADER]);
		if (!m_currentShaderFilenames[GEOMETRY_SHADER].empty()) addGeometryShader(m_currentShaderFilenames[GEOMETRY_SHADER]);
		if (!m_currentShaderFilenames[FRAGMENT_SHADER].empty()) addFragmentShader(m_currentShaderFilenames[FRAGMENT_SHADER]);

        link();
    }

    void renew() {
        (glDeleteProgram(m_program));
        m_program = GL(glCreateProgram());
    }

private:
    bool addShaderStageInternal(std::string filename, GLenum glType);

private:
	GLuint m_program = 0;

	std::string m_currentShaderFilenames[NUM_SHADERS_STAGES];
};

#endif // SHADER_H
