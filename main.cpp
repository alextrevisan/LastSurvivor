#include <stdio.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>
#include <Graphics.h>
#include <Joystick.h>
#include <TextureManager.h>

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

extern unsigned int tim_image[];
TIM_IMAGE tim_grass;

/* Function declarations */
void init();
void display();
struct UV
{
	unsigned char U,V,W,H;
};

struct ModelFlatShadedTextured
{
	SVECTOR vertices[8] = {
		{ -100, -100, -100, 0 },
		{  100, -100, -100, 0 },
		{ -100,  100, -100, 0 },
		{  100,  100, -100, 0 },
		{  100, -100,  100, 0 },
		{ -100, -100,  100, 0 },
		{  100,  100,  100, 0 },
		{ -100,  100,  100, 0 }
	};
	SVECTOR normals[8] = {
		{ 0, 0, -ONE, 0 },
		{ 0, 0, ONE, 0 },
		{ 0, -ONE, 0, 0 },
		{ 0, ONE, 0, 0 },
		{ -ONE, 0, 0, 0 },
		{ ONE, 0, 0, 0 }
	};
	INDEX vertices_index[8] = {
		{ 0, 1, 2, 3 },
		{ 4, 5, 6, 7 },
		{ 5, 4, 0, 1 },
		{ 6, 7, 3, 2 },
		{ 0, 2, 5, 7 },
		{ 3, 1, 6, 4 }
	};
	TIM_IMAGE* image;

};

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

	FntLoad(960, 0);
	FntOpen(0, 8, 320, 216, 0, 100);
	DrawSync(0);

	TextureManager::LoadTexture(tim_image, tim_grass);

	Joystick joystick = Joystick();
	
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
		

		if(joystick.Status() == 0)
		{
			if( joystick.IsDigital() || joystick.IsAnalog() || joystick.IsDualshock() )
			{
				if(joystick.IsPressed(PAD_LEFT))
				{
					rot.vy += 8;
				}
				if(joystick.IsPressed(PAD_RIGHT))
				{
					rot.vy -= 8;
				}

				if(joystick.IsPressed(PAD_DOWN))
				{
					rot.vx += 8;
				}
				if(joystick.IsPressed(PAD_UP))
				{
					rot.vx -= 8;
				}
			}
			//rot.vx += joystick.LeftStickX() - 128;
		}

		FntPrint(-1, "BUTTONS=%d\n", joystick.IsDigital());
		FntFlush(-1);		

		/* Make the cube SPEEN */
		
		//rot.vz += 8;
		
	
		
		for( i=0; i<CUBE_FACES; i++ ) {
			
			graphics->Draw<POLY_FT4>(
				{cube_verts[cube_indices[i].v0]	,
				cube_verts[cube_indices[i].v1],
				cube_verts[cube_indices[i].v2],
				cube_verts[cube_indices[i].v3]},
				cube_norms[i],
				&tim_grass
			);
		}

		//graphics->DrawFast<POLY_F4>(cube_verts, cube_indices, cube_norms, CUBE_FACES);

		/* Swap buffers and draw the primitives */
		graphics->Display<true>();
		
	}
	
	return 0;
	
}
