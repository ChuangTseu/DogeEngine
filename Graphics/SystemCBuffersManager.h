#pragma once

#include "GL.h"

#include "Doge.h"
#include "GlobalGraphicsDefines.h"

#include "MathsTools/mat4.h"
#include "MathsTools/vec3.h"
#include "MathsTools/vec4.h"

#define CB_PAD_BYTE(n) char CONCAT_MACRO(_pad_, __COUNTER__)[n];
#define CB_PAD_FLOAT(n) float CONCAT_MACRO(_pad_, __COUNTER__)[n];
#define CB_PAD_VEC4(n) vec4 CONCAT_MACRO(_pad_, __COUNTER__)[n];

struct PerFrameCB {
	vec3 m_randRgbColor;
	float m_time;
};

struct PerViewPointCB{
	mat4 g_view[MAX_NUM_VIEWPOINTPLANES];
	mat4 g_proj[MAX_NUM_VIEWPOINTPLANES];
	mat4 g_viewProj[MAX_NUM_VIEWPOINTPLANES];

	vec3 eyePosition;
	CB_PAD_FLOAT(1)
};

struct PerObjectCB{
	mat4 g_world;
};

struct DirLightCBStruct
{
	vec3 m_direction;
	CB_PAD_FLOAT(1)
	vec3 m_color;
	CB_PAD_FLOAT(1)
};

struct PointLightCBStruct {
	vec3 m_color;
	CB_PAD_FLOAT(1)
		vec3 m_position;
	CB_PAD_FLOAT(1)
};

struct PerLightCB{
	DirLightCBStruct dirLight;
	PointLightCBStruct pointLight;
};

struct PerMaterialCB{
	vec3 ka;
	CB_PAD_FLOAT(1)
	vec3 kd;
	CB_PAD_FLOAT(1)
	vec3 ks;
	float shininess;
};

struct PerFboCB{
	vec4 FboDimensions[16];
};

#define MAX_SHADER_LIGHTS 16

struct LightsCB{
	DirLightCBStruct dirLights[16];
	int numDirLights;
	CB_PAD_FLOAT(3);
};

struct UIConfigCB{
	int currentBRDF;

	float fresnel0;

	float roughnessOffset;

	bool filtering;
	bool diffuse;
	bool specularDirect;
	bool specularEnv;

	bool diffuseDirect;
	bool diffuseEnv;
	CB_PAD_BYTE(2)
};


class SystemCBuffersManager
{
public:
	static void Initialize();

public:

public:
	static PerFrameCB m_perFrameCb;
	static GLuint m_perFrameCbId;
	static PerViewPointCB m_perViewPointCb;
	static GLuint m_perViewPointId;
	static PerObjectCB m_perObjectCb;
	static GLuint m_perObjectCbId;
	static PerLightCB m_perLightCb;
	static GLuint m_perLightCbId;
	static PerMaterialCB m_perMaterialCb;
	static GLuint m_perMaterialCbId;
	static PerFboCB m_perFboCb;
	static GLuint m_perFboCbId;
	static LightsCB m_lightsCb;
	static GLuint m_lightsCbId;
	static UIConfigCB m_UIConfigCb;
	static GLuint m_UIConfigCbId;

public:
	static void CommitPerFrameCB();
	static void CommitPerViewPointCB();
	static void CommitPerObjectCB();
	static void CommitPerLightCB();
	static void CommitPerMaterialCB();
	static void CommitPerFboCB();
	static void CommitLightsCB();
	static void CommitUIConfigCB();

	static PerFrameCB& GetMutablePerFrameCB() { return m_perFrameCb; }
	static PerViewPointCB& GetMutablePerViewPointCB() { return m_perViewPointCb; }
	static PerObjectCB& GetMutablePerObjectCB() { return m_perObjectCb; }
	static PerLightCB& GetMutablePerLightCB() { return m_perLightCb; }
	static PerMaterialCB& GetMutablePerMaterialCB() { return m_perMaterialCb; }
	static PerFboCB& GetMutablePerFboCB() { return m_perFboCb; }
	static LightsCB& GetMutableLightsCB() { return m_lightsCb;  }
	static UIConfigCB& GetMutableUIConfigCB() { return m_UIConfigCb; }
};

