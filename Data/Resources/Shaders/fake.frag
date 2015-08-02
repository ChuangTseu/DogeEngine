//#version 420 core

in vec2 vTexCoord;

layout(location = 0, index = 0) out vec4 fragColor;

layout( std140, binding = 0 ) uniform PerFrameCB {
	vec3 g_randRgbColor;
   	float g_time;
};

void main( void )
{
	float mod2time = mod(g_time, 2.0);

	if (mod2time < 1.0)
	{
    	fragColor = vec4(vec3(mod2time), 1);
	}
	else
	{
    	fragColor = vec4(g_randRgbColor, 1);
	}
}
