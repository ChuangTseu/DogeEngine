#version 400 core

uniform int screenWidth;
uniform int screenHeight;

layout(location = 0, index = 0) out vec4 fragColor;

void main( void )
{
    vec2 screenTexCoord = vec2(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight);

    fragColor = vec4(screenTexCoord, 0, 1);
}
