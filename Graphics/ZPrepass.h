#pragma once

#include "Doge.h"

#include "GL.h"

#include "shader.h"
#include "PipelineState.h"

class ZPrepass
{
public:
	ZPrepass();
	~ZPrepass();

	Shader m_shader;
	PipelineState m_pipelineState;
};

