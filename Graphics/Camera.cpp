#include "camera.h"

Camera::Camera()
{
}

void Camera::SetPositionProperties(vec3 position, vec3 forward, vec3 up) {
    m_position = position;
    m_forward = forward;
    m_up = up;
}

void Camera::SetProjectionProperties(float fovY, float ratio, float near, float far)
{
	m_fovY = fovY;
	m_ratio = ratio;
	m_near = near;
	m_far = far;

	m_proj = mat4::Projection(fovY, ratio, near, far);
}

void Camera::UpdateLookAt()
{
	vec3 f = normalize(m_forward);
	vec3 s = normalize(cross(normalize(m_up), f));
	vec3 u = normalize(cross(f, s));

	m_pureViewNoTranslation = mat4{
		s.x, u.x, f.x, 0.0f,
		s.y, u.y, f.y, 0.0f,
		s.z, u.z, f.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	mat4 translation;
	translation.identity();
	translation.m_data[3][0] = -m_position.x;
	translation.m_data[3][1] = -m_position.y;
	translation.m_data[3][2] = -m_position.z;

	m_view = m_pureViewNoTranslation * translation;

	m_viewProj = m_proj * m_view;

	m_forward = f;
	//m_up = u;
}

void Camera::UpdateLookAtExplicit(const vec3& eye, const vec3& center, const vec3& up) {
    vec3 f = normalize(center - eye);
    vec3 s = normalize(cross(normalize(up), f));
    vec3 u = normalize(cross(f, s));

	m_pureViewNoTranslation = mat4{ 
		s.x, u.x, f.x, 0.0f,
		s.y, u.y, f.y, 0.0f,
		s.z, u.z, f.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f 
	};

    mat4 translation;
    translation.identity();
    translation.m_data[3][0] = -eye.x;
    translation.m_data[3][1] = -eye.y;
    translation.m_data[3][2] = -eye.z;

	m_view = m_pureViewNoTranslation * translation;

	m_viewProj = m_proj * m_view;

	m_position = eye;
	m_forward = f;
	//m_up = u;
}

const mat4& Camera::GetView() {
    return m_view;
}

const mat4& Camera::GetPureViewNoTranslation() {
    return m_pureViewNoTranslation;
}

const mat4& Camera::GetProjection() {
	return m_proj;
}

const mat4& Camera::GetViewProjection() {
	return m_viewProj;
}

void Camera::StepAdvance(EStepAdvance eStepAdvance)
{
	vec3 kright = normalize(cross(m_forward, m_up));
	vec3 kdown = normalize(cross(m_forward, kright));

	if (eStepAdvance == EStepAdvance_FORWARD) {
		m_position += m_forward*0.1f;
	}
	if (eStepAdvance == EStepAdvance_BACKWARD) {
		m_position -= m_forward*0.1f;
	}
	if (eStepAdvance == EStepAdvance_LEFT) {
		m_position -= kright*0.1f;
	}
	if (eStepAdvance == EStepAdvance_RIGHT) {
		m_position += kright*0.1f;
	}
	if (eStepAdvance == EStepAdvance_UP) {
		m_position += m_up*0.1f;
	}
	if (eStepAdvance == EStepAdvance_DOWN) {
		m_position -= m_up*0.1f;
	}
}

void Camera::RotateByMouseRelCoords(int mouse_x_rel, int mouse_y_rel)
{
	vec3 kright = normalize(cross(m_forward, m_up));
	vec3 kdown = normalize(cross(m_forward, kright));

	m_forward += kright*(mouse_x_rel / 100.f);
	m_forward += kdown*(mouse_y_rel / 100.f);

#pragma message("Maybe reenable if problem in per frame update")
	UpdateLookAt();
}

void Camera::TranslateByMouseRelCoords(int mouse_x_rel, int mouse_y_rel, int mouse_z_rel)
{
	vec3 kright = normalize(cross(m_forward, m_up));
	vec3 kdown = normalize(cross(m_forward, kright));

	m_position += kright*(-0.1f*mouse_x_rel);
	m_position += m_up*(0.1f*mouse_y_rel);
	m_position += m_forward*(0.1f*mouse_z_rel);
}
