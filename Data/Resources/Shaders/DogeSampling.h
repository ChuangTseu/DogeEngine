#pragma once

#define TS(i, j) textureOffset(t, uv, ivec2(i, j))
#define TS1(i, j) textureOffset(t, uv, ivec2(i, j)).r
#define TS2(i, j) textureOffset(t, uv, ivec2(i, j)).rg
#define TS3(i, j) textureOffset(t, uv, ivec2(i, j)).rgb
#define TS4(i, j) textureOffset(t, uv, ivec2(i, j)).rgba

vec3 Tap2x2(sampler2D t, vec2 uv)
{
	return (
		+TS3( 0, 0)	+TS3( 0, 1)
		+TS3( 1, 0)	+TS3( 1, 1)
	) / 4.0;
}

vec3 Tap4x4(sampler2D t, vec2 uv)
{
	return (
		+TS3( 0, 0)	+TS3( 0, 1)	+TS3( 0, 2)	+TS3( 0, 3)
		+TS3( 1, 0)	+TS3( 1, 1)	+TS3( 1, 2)	+TS3( 1, 3)
		+TS3( 2, 0)	+TS3( 2, 1)	+TS3( 2, 2)	+TS3( 2, 3)
		+TS3( 3, 0)	+TS3( 3, 1)	+TS3( 3, 2)	+TS3( 3, 3)
	) / 16.0;
}

#undef TS
#undef TS1
#undef TS2
#undef TS3
#undef TS4

