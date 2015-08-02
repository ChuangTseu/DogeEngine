//#version 420 core

in vec2 vTexCoord;

layout(location = 0, index = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D SceneColor;

void main( void )
{
	vec3 rgbInput = texture(SceneColor, vTexCoord).rgb;

    fragColor = vec4( rgbInput * vec3(1, 0.5, 0.5) , 1);
}
