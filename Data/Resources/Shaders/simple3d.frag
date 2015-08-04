//#version 420 core

#include "DogeCBuffers.h"

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec4 normalColor;
layout(location = 2) out vec4 texcoordColor;

in Data {
    vec3 position;
    vec2 texcoord;
    vec3 normal;
    vec3 tangent;
} inData;

vec3 V;

vec3 B;
vec3 T;
vec3 N;

#define approx(val, target, delta) ( (val <= (target + delta) ) && (val >= (target - delta) ) )

bool vec_approx(vec3 v, vec3 target, float delta) {
    return (approx(v.x, target.x, delta) && approx(v.y, target.y, delta) && approx(v.z, target.z, delta));
}

vec3 blinn_phong_calc_internal(vec3 lightDir, vec3 lightColor, vec3 normal) {
    float Id = clamp(dot(normal, lightDir), 0, 1);

    vec3 viewDir = normalize(g_eyePosition - inData.position);
    vec3 halfV = normalize(lightDir + viewDir);

    float Is = 0;
    if (Id > 0) {
        Is = pow(clamp(dot(normal, halfV), 0, 1), shininess);
    }

    // Replace the 3 colors by light ambiant, diffuse and specular intensity respectively
    float tmpAmbientFactor = 0.2;
    return Is * lightColor*Id;
}

vec3 blinn_phong_calc(DirLight light, vec3 normal) {
    vec3 lightDir = normalize(-light.direction);

    return blinn_phong_calc_internal(lightDir, light.color, normal);
}

vec3 blinn_phong_calc(PointLight light, vec3 normal) {
    vec3 lightDir = normalize(light.position - g_eyePosition);

    return blinn_phong_calc_internal(lightDir, light.color, normal);
}

void main( void )
{
    V = normalize(g_eyePosition - inData.position);

    vec3 worldNormal = normalize(inData.normal);
    vec3 worldTangent = normalize(inData.tangent);
    vec3 worldBitangent = normalize(cross(worldNormal, worldTangent));

    N = worldNormal;
    T = worldTangent;
    B = worldBitangent;

    normalColor = vec4(N, 1);
    texcoordColor = vec4(inData.texcoord, 0, 1);

    //vec3 pointL = normalize(pointLight.position - inData.position);

   	vec3 pointL = normalize(vec3(cos(g_time), 0, sin(g_time)));

    vec3 pointH = normalize(pointL + V);


    vec3 H = normalize(pointL + V);

    vec3 L = pointL;

    vec3 C = vec3(0.72549019607, 0.94901960784, 1.0); // Diamond color


    float NdotH = dot(N, H);
    float NdotV = dot(N, V);
    float VdotH = dot(V, H);
    float LdotH = dot(L, H);
    float NdotL = dot(N, L);

    float fr = 0;

    vec3 finalColor = vec3(0);

  	DirLight testDirLight;
	testDirLight.direction = L;
	testDirLight.color = vec3(1,0,1);

    finalColor += blinn_phong_calc(testDirLight, N);

    for (int nDirLight = 0; nDirLight < g_numDirLights; ++nDirLight)
    {
        finalColor += blinn_phong_calc(g_dirLights[nDirLight], N);
    }

//    fragColor.xyz += vec3(0.05); // FAKE AMBIANT

    fragColor = vec4(finalColor, 1);
}
