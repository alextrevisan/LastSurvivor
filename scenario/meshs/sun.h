#ifndef __sun_h_
#define __sun_h_

#include <psxgte.h>

struct sun {

	const SVECTOR vertices[4] = {
		{ 18821, 1000, -1000 },
		{ 18821, -1000, -1000 },
		{ 18821, 1000, 1000 },
		{ 18821, -1000, 1000 },
	};

	const DVECTOR uvs[4] = {
		{ 0, 63 },
		{ 63, 63 },
		{ 63, 0 },
		{ 0, 0 },
	};

	const SVECTOR normals[1] = {
		{ -4095, 0, 0 },
	};

	struct face4
	{
		short vertice0,uv0,normal0;
		short vertice1,uv1,normal1;
		short vertice2,uv2,normal2;
		short vertice3,uv3,normal3;
	};

	const face4 quads[1] = {
		{ 0, 0, 0, 1, 1, 0, 3, 2, 0, 2, 3, 0 },
	};

	struct face3
	{
		unsigned short vertice0,uv0,normal0;
		unsigned short vertice1,uv1,normal1;
		unsigned short vertice2,uv2,normal2;
	};

	const face3 tris[0] = {
	};

	int x = 0, y = 0, z = 0;
	TIM_IMAGE* texture = nullptr;
};

#endif //__sun_h_
