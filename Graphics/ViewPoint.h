#pragma once

#include "Doge.h"

#include "GlobalGraphicsDefines.h"

#include "SystemCBuffersManager.h"

#include "MathsTools/mat4.h"
#include "MathsTools/vec3.h"
#include "MathsTools/maths.h"

struct ViewPointPlane
{
	mat4 View;
};

class ViewPoint
{
public:
	void Bind() const;

	mat4 m_view[MAX_NUM_VIEWPOINTPLANES];
	mat4 m_proj[MAX_NUM_VIEWPOINTPLANES];
	mat4 m_viewProj[MAX_NUM_VIEWPOINTPLANES];

	vec3 m_eyePosition;
};

