//#version 420 core

#include "DogeCBuffers.h"
#include "DogeSampling.h"

in vec2 vTexCoord;

layout(location = 0, index = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D SceneColor;

void main( void )
{
	const float offsetU = -0.5 / (g_FboDimensions[0].x * 2);
	const float offsetV = -0.5 / (g_FboDimensions[0].y * 2);

	const vec2 vOffset = vec2(offsetU, offsetV);

    fragColor = vec4( Tap2x2(SceneColor, vTexCoord + vOffset) , 1);
}
