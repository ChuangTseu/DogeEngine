#version 400 core

uniform sampler2D texSampler;

uniform int screenWidth;
uniform int screenHeight;

//layout(location = 0, index = 0).x out vec4 fragColor;

layout(location = 1) out vec4 leadr1;
layout(location = 2) out vec4 leadr2;

#define approx(x, val) (val - 0.00001 < x && x < val + 0.00001)


float luminance(vec3 color) {
    return color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0722;
}

void main( void )
{
    vec2 screenTexCoord = vec2(gl_FragCoord.x/screenWidth, gl_FragCoord.y/screenHeight);

    vec3 rgbColor = texture(texSampler, screenTexCoord).rgb;


    float Gdx = 0;
    float Gdy = 0;

    Gdx += -texelFetch(texSampler, ivec2(gl_FragCoord.x-1, gl_FragCoord.y+1), 0).x;
    Gdx -= -texelFetch(texSampler, ivec2(gl_FragCoord.x+1, gl_FragCoord.y+1), 0).x;
    Gdx += -texelFetch(texSampler, ivec2(gl_FragCoord.x-1, gl_FragCoord.y-1), 0).x;
    Gdx -= -texelFetch(texSampler, ivec2(gl_FragCoord.x+1, gl_FragCoord.y-1), 0).x;
    Gdx += -2*texelFetch(texSampler, ivec2(gl_FragCoord.x-1, gl_FragCoord.y), 0).x;
    Gdx -= -2*texelFetch(texSampler, ivec2(gl_FragCoord.x+1, gl_FragCoord.y), 0).x;

    Gdy += -texelFetch(texSampler, ivec2(gl_FragCoord.x-1, gl_FragCoord.y-1), 0).x;
    Gdy += -2*texelFetch(texSampler, ivec2(gl_FragCoord.x, gl_FragCoord.y-1), 0).x;
    Gdy += -texelFetch(texSampler, ivec2(gl_FragCoord.x+1, gl_FragCoord.y-1), 0).x;
    Gdy -= -texelFetch(texSampler, ivec2(gl_FragCoord.x-1, gl_FragCoord.y+1), 0).x;
    Gdy -= -2*texelFetch(texSampler, ivec2(gl_FragCoord.x, gl_FragCoord.y+1), 0).x;
    Gdy -= -texelFetch(texSampler, ivec2(gl_FragCoord.x+1, gl_FragCoord.y+1), 0).x;

    float disp = rgbColor.x;

    leadr1 = vec4(Gdx, Gdy, Gdx*Gdy, Gdx*Gdx);
    leadr2 = vec4(Gdy*Gdy, disp, 0, 0);

}
