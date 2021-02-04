#include <stdio.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>
#include <Graphics.h>

/* Cube vertices */
SVECTOR cube_verts[] = {
	{ -100, -100, -100, 0 },
	{  100, -100, -100, 0 },
	{ -100,  100, -100, 0 },
	{  100,  100, -100, 0 },
	{  100, -100,  100, 0 },
	{ -100, -100,  100, 0 },
	{  100,  100,  100, 0 },
	{ -100,  100,  100, 0 }
};

/* Cube face normals */
SVECTOR cube_norms[] = {
	{ 0, 0, -ONE, 0 },
	{ 0, 0, ONE, 0 },
	{ 0, -ONE, 0, 0 },
	{ 0, ONE, 0, 0 },
	{ -ONE, 0, 0, 0 },
	{ ONE, 0, 0, 0 }
};

/* Cube vertex indices */
INDEX cube_indices[] = {
	{ 0, 1, 2, 3 },
	{ 4, 5, 6, 7 },
	{ 5, 4, 0, 1 },
	{ 6, 7, 3, 2 },
	{ 0, 2, 5, 7 },
	{ 3, 1, 6, 4 }
};

/* Number of faces of cube */
#define CUBE_FACES 6


/* Light color matrix */
/* Each column represents the color matrix of each light source and is */
/* used as material color when using gte_ncs() or multiplied by a */
/* source color when using gte_nccs(). 4096 is 1.0 in this matrix */
/* A column of zeroes disables the light source. */
MATRIX color_mtx = {
	ONE, 0, 0,	/* Red   */
	ONE, 0, 0,	/* Green */
	ONE, 0, 0	/* Blue  */
};

/* Light matrix */
/* Each row represents a vector direction of each light source. */
/* An entire row of zeroes disables the light source. */
MATRIX light_mtx = {
	/* X,  Y,  Z */
	-2048 , -2048 , -2048,
	0	  , 0	  , 0,
	0	  , 0	  , 0
};


/* Function declarations */
void init();
void display();


/* Main function */
int main() {
	GraphClass<256,1024> *graphics = new GraphClass<256,1024>();
	int i,p,xy_temp;
	
	SVECTOR	rot = { 0 };			/* Rotation vector for Rotmatrix */
	VECTOR	pos = { 0, 0, 400 };	/* Translation vector for TransMatrix */
	MATRIX	mtx,lmtx;				/* Rotation matrices for geometry and lighting */
	
	POLY_F4	*pol4;					/* Flat shaded quad primitive pointer */
	
	
	/* Init graphics and GTE */
	graphics->SetResolution(320,240);
	gte_SetColorMatrix( &color_mtx );
	
	
	/* Main loop */
	while( 1 ) {
		
		/* Set rotation and translation to the matrix */
		RotMatrix( &rot, &mtx );
		TransMatrix( &mtx, &pos );
		
		/* Multiply light matrix by rotation matrix so light source */
		/* won't appear relative to the model's rotation */
		MulMatrix0( &light_mtx, &mtx, &lmtx );
		
		/* Set rotation and translation matrix */
		gte_SetRotMatrix( &mtx );
		gte_SetTransMatrix( &mtx );
		
		/* Set light matrix */
		gte_SetLightMatrix( &lmtx );
		
		/* Make the cube SPEEN */
		rot.vx += 1;
		rot.vz += 1;
		
	
		
		/*for( i=0; i<CUBE_FACES; i++ ) {
			
			graphics->DrawFast<POLY_F4>(
				{cube_verts[cube_indices[i].v0]	,
				cube_verts[cube_indices[i].v1],
				cube_verts[cube_indices[i].v2],
				cube_verts[cube_indices[i].v3]},
				cube_norms[i]
			);
		}*/

		graphics->DrawFast<POLY_F4>(cube_verts, cube_indices, cube_norms, CUBE_FACES);
		
		/* Update nextpri variable */
		/* (IMPORTANT if you plan to sort more primitives after this) */
		//graphics->SetPri((u_char*)pol4);
		
		/* Swap buffers and draw the primitives */
		graphics->Display<true>();
		
	}
	
	return 0;
	
}
