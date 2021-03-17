#ifndef __dead_tree_h_
#define __dead_tree_h_

#include <psxgte.h>

struct dead_tree {

	const SVECTOR vertices[10] = {
		{ -740, 0, 0 },
		{ 740, 0, 0 },
		{ -740, -2424, 0 },
		{ 740, -2424, 0 },
		{ 0, 0, 0 },
		{ 0, -2424, 0 },
		{ 0, 0, -740 },
		{ 0, 0, 740 },
		{ 0, -2424, -740 },
		{ 0, -2424, 740 },
	};

	const DVECTOR uvs[10] = {
		{ 32, 127 },
		{ 0, 127 },
		{ 0, 0 },
		{ 32, 0 },
		{ 63, 127 },
		{ 63, 0 },
		{ 0, 127 },
		{ 0, 0 },
		{ 63, 127 },
		{ 63, 0 },
	};

	const SVECTOR normals[2] = {
		{ 0, 0, -4095 },
		{ 4095, 0, 0 },
	};

	struct face4
	{
		short vertice0,uv0,normal0;
		short vertice1,uv1,normal1;
		short vertice2,uv2,normal2;
		short vertice3,uv3,normal3;
	};

	const face4 quads[4] = {
		{ 4, 0, 0, 1, 1, 0, 3, 2, 0, 5, 3, 0 },
		{ 0, 4, 0, 4, 0, 0, 5, 3, 0, 2, 5, 0 },
		{ 4, 0, 1, 7, 6, 1, 9, 7, 1, 5, 3, 1 },
		{ 6, 8, 1, 4, 0, 1, 5, 3, 1, 8, 9, 1 },
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

#endif //__dead_tree_h_
