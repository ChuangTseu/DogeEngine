#ifndef CAMERA_H
#define CAMERA_H

#include "MathsTools/mat4.h"
#include "MathsTools/vec3.h"
#include "MathsTools/maths.h"

#include "GL.h"

#include "ViewPoint.h"

class Camera
{
public:
    Camera();

	void SetPositionProperties(vec3 position, vec3 forward, vec3 up);

	void SetProjectionProperties(float fovY, float ratio, float near, float far);

	void UpdateLookAt();

	void UpdateLookAtExplicit(const vec3& eye, const vec3& center, const vec3& up);

	ViewPoint ComputeViewPoint() const
	{
		ViewPoint cameraViewPoint;

		cameraViewPoint.m_view[0] = m_view;
		cameraViewPoint.m_proj[0] = m_proj;
		cameraViewPoint.m_viewProj[0] = m_viewProj;

		cameraViewPoint.m_eyePosition = m_position;

		return cameraViewPoint;
	}

    const mat4& GetView();
    const mat4& GetPureViewNoTranslation();

	const mat4& GetProjection();
	const mat4& GetViewProjection();

	enum EStepAdvance {
		EStepAdvance_FORWARD,
		EStepAdvance_BACKWARD,
		EStepAdvance_LEFT,
		EStepAdvance_RIGHT,
		EStepAdvance_DOWN,
		EStepAdvance_UP
	};

	void StepAdvance(EStepAdvance eStepAdvance);

	void RotateByMouseRelCoords(int mouse_x_rel, int mouse_y_rel);
	void TranslateByMouseRelCoords(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel);

private:
	mat4 m_view;
	mat4 m_proj;
	mat4 m_viewProj;

	//WARNING, No translation
	mat4 m_pureViewNoTranslation;

	vec3 m_position;
	vec3 m_forward;
	vec3 m_up;

	float m_fovY;
	float m_ratio;
	float m_near;
	float m_far;
};

#endif // CAMERA_H
