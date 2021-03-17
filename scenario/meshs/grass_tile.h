#ifndef __grass_tile_h_
#define __grass_tile_h_

#include <psxgte.h>

struct grass_tile {

	const SVECTOR vertices[4] = {
		{ -800, 0, -800 },
		{ 800, 0, -800 },
		{ -800, 0, 800 },
		{ 800, 0, 800 },
	};

	const DVECTOR uvs[4] = {
		{ 62, 0 },
		{ 62, 62 },
		{ 0, 0 },
		{ 0, 62 },
	};

	const SVECTOR normals[1] = {
		{ 0, -4094, 0 },
	};

	struct face4
	{
		short vertice0,uv0,normal0;
		short vertice1,uv1,normal1;
		short vertice2,uv2,normal2;
		short vertice3,uv3,normal3;
	};

	const face4 quads[0] = {
	};

	struct face3
	{
		unsigned short vertice0,uv0,normal0;
		unsigned short vertice1,uv1,normal1;
		unsigned short vertice2,uv2,normal2;
	};

	const face3 tris[2] = {
		{ 1, 0, 0, 3, 1, 0, 0, 2, 0 },
		{ 0, 2, 0, 3, 1, 0, 2, 3, 0 },
	};

	int x = 0, y = 0, z = 0;
	TIM_IMAGE* texture = nullptr;
};

#endif //__grass_tile_h_
