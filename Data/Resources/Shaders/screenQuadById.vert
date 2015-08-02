//#version 420 core

out vec2 vTexCoord;

void main( void )
{
	vTexCoord   = vec2( (gl_VertexID << 1) & 2, gl_VertexID & 2 );
	gl_Position = vec4( vTexCoord * 2.0 - 1.0, 0.0, 1.0 );
}
