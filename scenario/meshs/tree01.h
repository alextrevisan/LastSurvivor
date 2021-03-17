#ifndef __tree01_h_
#define __tree01_h_

#include <psxgte.h>

struct tree01 {

	const SVECTOR vertices[0] = {
	};

	const DVECTOR uvs[0] = {
	};

	const SVECTOR normals[0] = {
	};

	struct face4
	{
		unsigned short vertice0,uv0,normal0;
		unsigned short vertice1,uv1,normal1;
		unsigned short vertice2,uv2,normal2;
		unsigned short vertice3,uv3,normal3;
	};

	const face4 quads[0] = {
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

#endif //__tree01_h_
