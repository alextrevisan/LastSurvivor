#ifndef __clouds_h_
#define __clouds_h_

#include <psxgte.h>

struct clouds {

	const SVECTOR vertices[9] = {
		{ -8619, -5079, -8619 },
		{ 8619, -5079, -8619 },
		{ -8619, -5079, 8619 },
		{ 8619, -5079, 8619 },
		{ -8619, -5079, 0 },
		{ 8619, -5079, 0 },
		{ 0, -5079, -8619 },
		{ 0, -5079, 8619 },
		{ 0, -5079, 0 },
	};

	const DVECTOR uvs[9] = {
		{ 32, 31 },
		{ 32, 0 },
		{ 64, 0 },
		{ 64, 31 },
		{ 32, 63 },
		{ 64, 63 },
		{ 0, 63 },
		{ 0, 31 },
		{ 0, 0 },
	};

	const SVECTOR normals[1] = {
		{ 0, 4095, 0 },
	};

	struct face4
	{
		short vertice0,uv0,normal0;
		short vertice1,uv1,normal1;
		short vertice2,uv2,normal2;
		short vertice3,uv3,normal3;
	};

	const face4 quads[4] = {
		{ 8, 0, 0, 7, 1, 0, 3, 2, 0, 5, 3, 0 },
		{ 6, 4, 0, 8, 0, 0, 5, 3, 0, 1, 5, 0 },
		{ 0, 6, 0, 4, 7, 0, 8, 0, 0, 6, 4, 0 },
		{ 4, 7, 0, 2, 8, 0, 7, 1, 0, 8, 0, 0 },
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

#endif //__clouds_h_
