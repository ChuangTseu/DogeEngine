#pragma once

#define MAX_NUM_VIEWPOINTPLANES 6

layout( std140, binding = 0 ) uniform PerFrameCB {
	vec3 g_randRgbColor;
   	float g_time;
};

layout( std140, binding = 1 ) uniform PerViewPointCB {
   	mat4 g_view[MAX_NUM_VIEWPOINTPLANES];
	mat4 g_proj[MAX_NUM_VIEWPOINTPLANES];
	mat4 g_viewProj[MAX_NUM_VIEWPOINTPLANES];

	vec3 g_eyePosition; // TODO : EXTRACT FROM VIEW
};

layout( std140, binding = 2 ) uniform PerObjectCB {
   	mat4 g_world;
};

struct DirLight {
    vec3 direction;
    vec3 color;
};

struct PointLight {
    vec3 position;
    vec3 color;
};

layout( std140, binding = 3 ) uniform PerLightCB {
   	DirLight dirLight;
   	PointLight pointLight;
};

layout( std140, binding = 4 ) uniform PerMaterialCB {
	vec3 ka;
	vec3 kd;
	vec3 ks;
	float shininess;
};

layout( std140, binding = 5 ) uniform PerFboCB {
   	vec4 g_FboDimensions[16]; // x : Width, y : Height, z : 1 / Width, w : 1 / Height
};

#define MAX_SHADER_LIGHTS 16

layout( std140, binding = 6 ) uniform LightsCB {
   	DirLight g_dirLights[MAX_SHADER_LIGHTS];
   	int g_numDirLights;
};

layout( std140, binding = 8 ) uniform UIConfigCB {
	int currentBRDF;

	float fresnel0;

	float roughnessOffset;

	bool filtering;
	bool diffuse;
	bool specularDirect;
	bool specularEnv;

	bool diffuseDirect;
	bool diffuseEnv;
};
