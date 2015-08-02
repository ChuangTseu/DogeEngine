#version 400 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 3) in vec3 tangent;

out Data {
    vec3 pos;
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;
} outData;

uniform mat4 world;

void main( void )
{
    outData.pos =  (world * vec4(vertex, 1)).xyz;
    outData.normal = (world * vec4(normal, 0)).xyz;
    outData.texcoord = texcoord;
    outData.tangent = (world * vec4(tangent, 0)).xyz;
}
