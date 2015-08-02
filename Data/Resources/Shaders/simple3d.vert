//#version 420 core

#include "DogeCBuffers.h"

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;

out Data {
    vec3 position;
    vec2 texcoord;
    vec3 normal;
    vec3 tangent;
} outData;

void main( void )
{
    outData.position =  (g_world * vec4(vertex, 1)).xyz;
    outData.normal = (g_world * vec4(normal, 0)).xyz;
    outData.texcoord = texcoord;
    outData.tangent = (g_world * vec4(tangent, 0)).xyz;

    gl_Position = g_proj[0] * g_view[0] * vec4(outData.position, 1.0);
}
