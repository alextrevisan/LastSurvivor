#ifndef __cubo_h_
#define __cubo_h_

#include <psxgte.h>

struct cubo {

	const SVECTOR vertices[8] = {
		{ 100, -100, 100 },
		{ 100, 100, 100 },
		{ 100, -100, -100 },
		{ 100, 100, -100 },
		{ -100, -100, 100 },
		{ -100, 100, 100 },
		{ -100, -100, -100 },
		{ -100, 100, -100 },
	};

	const DVECTOR uvs[20] = {
		{ 0, 0 },
		{ 64, 0 },
		{ 64, 64 },
		{ 0, 64 },
		{ 0, 0 },
		{ 64, 0 },
		{ 0, 64 },
		{ 0, 0 },
		{ 64, 0 },
		{ 64, 64 },
		{ 0, 64 },
		{ 0, 0 },
		{ 64, 0 },
		{ 64, 64 },
		{ 0, 0 },
		{ 64, 0 },
		{ 64, 64 },
		{ 0, 64 },
		{ 64, 64 },
		{ 0, 64 },
	};

	const SVECTOR normals[6] = {
		{ 0, -4069, 0 },
		{ 0, 0, -4069 },
		{ -4069, 0, 0 },
		{ 0, 4069, 0 },
		{ 4069, 0, 0 },
		{ 0, 0, 4069 },
	};

	struct face4
	{
		unsigned char vertice0,uv0,normal0;
		unsigned char vertice1,uv1,normal1;
		unsigned char vertice2,uv2,normal2;
		unsigned char vertice3,uv3,normal3;
	};

	const face4 quads[6] = {
		{ 0, 0, 0, 4, 1, 0, 6, 2, 0, 2, 3, 0 },
		{ 3, 4, 1, 2, 5, 1, 6, 2, 1, 7, 6, 1 },
		{ 7, 7, 2, 6, 8, 2, 4, 9, 2, 5, 10, 2 },
		{ 5, 11, 3, 1, 12, 3, 3, 13, 3, 7, 6, 3 },
		{ 1, 14, 4, 0, 15, 4, 2, 16, 4, 3, 17, 4 },
		{ 5, 11, 5, 4, 1, 5, 0, 18, 5, 1, 19, 5 },
	};

	struct face3
	{
		unsigned char vertice0,uv0,normal0;
		unsigned char vertice1,uv1,normal1;
		unsigned char vertice2,uv2,normal2;
		unsigned char vertice3,uv3,normal3;
	};

	const face3 tris[0] = {
	};

	int x = 0, y = 0, z = 0;
	TIM_IMAGE* texture = nullptr;
};

#endif //__cubo_h_
