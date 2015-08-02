#version 410 core

layout(triangles, equal_spacing, cw) in;

out Data {
    vec3 normal;
    vec2 texcoord;
    vec3 tangent;

    vec3 position;
} outData;

// Uniform

uniform mat4 view;
uniform mat4 projection;

uniform mat4 world;

uniform sampler2D leadr2; //~y², h
uniform float userDisplacementFactor;



struct outputPatch
{
    vec3 worldPos_B030;
    vec3 worldPos_B021;
    vec3 worldPos_B012;
    vec3 worldPos_B003;
    vec3 worldPos_B102;
    vec3 worldPos_B201;
    vec3 worldPos_B300;
    vec3 worldPos_B210;
    vec3 worldPos_B120;
    vec3 worldPos_B111;
    vec3 normal[3];
    vec3 tangent[3];
    vec2 texCoord[3];
};


patch in outputPatch oPatch;


vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    // Interpolate the attributes of the output vertex using the barycentric coordinates
    outData.texcoord = interpolate2D(oPatch.texCoord[0], oPatch.texCoord[1], oPatch.texCoord[2]);
    outData.normal = interpolate3D(oPatch.normal[0], oPatch.normal[1], oPatch.normal[2]);
    outData.tangent = interpolate3D(oPatch.tangent[0], oPatch.tangent[1], oPatch.tangent[2]);



    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    float uPow3 = u*u*u;
    float vPow3 = v*v*v;
    float wPow3 = w*w*w;
    float uPow2 = u*u;
    float vPow2 = v*v;
    float wPow2 = w*w;

//    outData.position =
//            oPatch.worldPos_B300 * wPow3 +
//            oPatch.worldPos_B030 * uPow3 +
//            oPatch.worldPos_B003 * vPow3 +
//            oPatch.worldPos_B210 * 3.0 * wPow2 * u +
//            oPatch.worldPos_B120 * 3.0 * w * uPow2 +
//            oPatch.worldPos_B201 * 3.0 * wPow2 * v +
//            oPatch.worldPos_B021 * 3.0 * uPow2 * v +
//            oPatch.worldPos_B102 * 3.0 * w * vPow2 +
//            oPatch.worldPos_B012 * 3.0 * u * vPow2 +
//            oPatch.worldPos_B111 * 6.0 * w * u * v;

            outData.position =
                    oPatch.worldPos_B300 * w +
                    oPatch.worldPos_B030 * u +
                    oPatch.worldPos_B003 * v
              ;

   float disp = texture(leadr2, outData.texcoord).y;
    outData.position +=  outData.normal*disp*userDisplacementFactor;
    gl_Position = projection * view * vec4(outData.position, 1.0);

}




