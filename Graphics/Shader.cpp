#include "shader.h"

#include "../helpers.h"

bool Shader::addVertexShader(std::string filename) {
	m_currentShaderFilenames[VERTEX_SHADER] = filename;
    return addShaderStageInternal(filename, GL_VERTEX_SHADER);
}

bool Shader::addFragmentShader(std::string filename) {
	m_currentShaderFilenames[FRAGMENT_SHADER] = filename;
    return addShaderStageInternal(filename, GL_FRAGMENT_SHADER);
}

bool Shader::addTessControlShader(std::string filename) {
	m_currentShaderFilenames[TESS_CONTROL_SHADER] = filename;
    return addShaderStageInternal(filename, GL_TESS_CONTROL_SHADER);
}

bool Shader::addTessEvaluationShader(std::string filename) {
	m_currentShaderFilenames[TESS_EVAL_SHADER] = filename;
    return addShaderStageInternal(filename, GL_TESS_EVALUATION_SHADER);
}

bool Shader::addGeometryShader(std::string filename) {
	m_currentShaderFilenames[GEOMETRY_SHADER] = filename;
    return addShaderStageInternal(filename, GL_GEOMETRY_SHADER);
}

#include <string>
#include <iostream>
#include <stdio.h>

#if WIN32
#define popen _popen
#define pclose _pclose
#endif

std::string ExecCmd(const char* cmd) {
	FILE* pipe = popen(cmd, "r");
	if (!pipe) return "ERROR";
	char buffer[128];
	std::string result = "";
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}
	pclose(pipe);
	return result;
}

bool Shader::addShaderStageInternal(std::string filename, GLenum glType) {
    GLint shader_compiled;

    GLuint shaderId = GL(glCreateShader(glType));

	std::string strShadersResourcesFolder = "Data/Resources/Shaders/";

	std::string strShaderFinalFilename = strShadersResourcesFolder + filename;

#if WIN32
	std::string strMcppCommand = std::string("Data\\ExtTools\\mcpp.exe -P ") + "-I" + strShadersResourcesFolder + " " + strShaderFinalFilename;
#else
	DogeAssertAlways(); // Todo use linux version
#endif

	std::string strShaderSource;
	strShaderSource += "#version 420 core\n";
	strShaderSource += ExecCmd(strMcppCommand.c_str());

	GLint length = strShaderSource.size();;
	const GLchar* source = strShaderSource.c_str();

    GL(glShaderSource(shaderId, 1, (const char**)&source, &length));

	GL(glCompileShader(shaderId));

    GL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shader_compiled));

    GLsizei log_length = 0;
    GLchar message[4096];
    GL(glGetShaderInfoLog(shaderId, 4096, &log_length, message));

    if (shader_compiled != GL_TRUE)
    {
        std::cerr << "Error compiling shader " << filename << ": " << message << '\n';

        return false;
    }
    else if(strlen(message)) {
        std::cerr << "Although compiled with success, shader " << filename << " has following compile output: " << message << '\n';
    }


    GL(glAttachShader(m_program, shaderId));

    return true;
}

bool Shader::link() {
    GL(glLinkProgram(m_program));

    GLint program_linked;
    GL(glGetProgramiv(m_program, GL_LINK_STATUS, &program_linked));
    if (program_linked != GL_TRUE)
    {
        GLsizei log_length = 0;
        GLchar message[1024];
        GL(glGetProgramInfoLog(m_program, 1024, &log_length, message));

        std::cerr << "Error linking program: " << message;

        return false;
    }

    return true;
}

void Shader::use() const
{
    GL(glUseProgram(m_program));
}

void Shader::sendTransformations(const mat4& projection, const mat4& view, const mat4& model) {
    mat4 MVP = projection * view * model;

    GL(glUniformMatrix4fv(glGetUniformLocation(m_program, "MVP"), 1, GL_FALSE, MVP.data()));

    GL(glUniformMatrix4fv(glGetUniformLocation(m_program, "world"), 1, GL_FALSE, model.data()));

    GL(glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, GL_FALSE, projection.data()));
    GL(glUniformMatrix4fv(glGetUniformLocation(m_program, "view"), 1, GL_FALSE, view.data()));
    //    GL(glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, GL_FALSE, model.data()));
}

void Shader::sendMaterial(const Material &mat)
{
    GL(glUniform3fv(glGetUniformLocation(m_program, "ka"), 1, mat.m_ambientReflectance.data()));
    GL(glUniform3fv(glGetUniformLocation(m_program, "kd"), 1, mat.m_diffuseReflectance.data()));
    GL(glUniform3fv(glGetUniformLocation(m_program, "ks"), 1, mat.m_specularReflectance.data()));

    GL(glUniform1f(glGetUniformLocation(m_program, "shininess"), mat.m_specularExponent));

    mat.m_diffuseTexture.bindToTarget(GL_TEXTURE0);
    mat.m_normalTexture.bindToTarget(GL_TEXTURE1);

}
