//#version 420 core

#include "DogeCBuffers.h"
#include "DogeSampling.h"

in vec2 vTexCoord;

layout(location = 0, index = 0) out vec4 fragColor;

layout(binding = 0) uniform sampler2D SceneColor;

void main( void )
{
    fragColor = vec4( 1, 0, 0 , 1);
}
