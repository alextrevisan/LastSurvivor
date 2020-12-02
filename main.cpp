
#include <stdio.h>
#include <stdlib.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <psxpad.h>
#include <psxapi.h>
#include <psxetc.h>
#include <inline_c.h>

#include "PerlinNoise.h"

#include "clip.h"
#include "lookat.h"
#include "camera.h"

#include <Map.h>
#include "animated/character1.h"
// OT and Packet Buffer sizes
#define OT_LEN			1024
#define PACKET_LEN		32768

// Screen resolution
#define SCREEN_XRES		320
#define SCREEN_YRES		240

// Screen center position
#define CENTERX			SCREEN_XRES>>1
#define CENTERY			SCREEN_YRES>>1

//#include <array.h>
// Double buffer structure
typedef struct {
	DISPENV	disp;			// Display environment
	DRAWENV	draw;			// Drawing environment
	unsigned int 	ot[OT_LEN];		// Ordering table
	char 	p[PACKET_LEN];	// Packet buffer
} DB;

// Double buffer variables
DB		db[2];
int		db_active = 0;
char	*db_nextpri;
RECT	screen_clip;

// Pad data buffer
char pad_buff[2][34];


// For easier handling of vertex indexes
typedef struct {
	short v0,v1,v2,v3;
} INDEX;

// Cube vertices
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

// Cube face normals
SVECTOR cube_norms[] = {
	{ 0, 0, -ONE, 0 },
	{ 0, 0, ONE, 0 },
	{ 0, -ONE, 0, 0 },
	{ 0, ONE, 0, 0 },
	{ -ONE, 0, 0, 0 },
	{ ONE, 0, 0, 0 }
};

// Cube vertex indices
INDEX cube_indices[] = {
	{ 0, 1, 2, 3 },
	{ 4, 5, 6, 7 },
	{ 5, 4, 0, 1 },
	{ 6, 7, 3, 2 },
	{ 0, 2, 5, 7 },
	{ 3, 1, 6, 4 }
};

// Number of faces of cube
#define CUBE_FACES 6


// Light color matrix
// Each column represents the color matrix of each light source and is
// used as material color when using gte_ncs() or multiplied by a
// source color when using gte_nccs(). 4096 is 1.0 in this matrix
// A column of zeroes effectively disables the light source.
MATRIX color_mtx = {
	ONE, 0, 0,	// Red
	ONE, 0, 0,	// Green
	ONE, 0, 0	// Blue
};

// Light matrix
// Each row represents a vector direction of each light source.
// An entire row of zeroes effectively disables the light source.
MATRIX light_mtx = {
	/* X,  Y,  Z */
	1755 , -1778 , -2000,
	0 , 0 , 0,
	0 , 0 , 0
};


// Function declarations
void init();
void display();

void sort_cube(MATRIX *mtx, VECTOR *pos, SVECTOR *rot);
PerlinNoise perlin;

POLY_F3 *renderTriangle(const SVECTOR* tri, const SVECTOR* normal, const CVECTOR& color, POLY_F3* pol4)
{
	int p;
	gte_ldv3(&tri[0], &tri[1], &tri[2] );
	// Rotation, Translation and Perspective Triple
	gte_rtpt();
	// Compute normal clip for backface culling
	gte_nclip();
	// Get result
	gte_stopz( &p );
	// Skip this face if backfaced
	if( p < 0 )
		return pol4;
	// Calculate average Z for depth sorting
	gte_avsz3();
	gte_stotz( &p );
	// Skip if clipping off
	// (the shift right operator is to scale the depth precision)
	if( ((p>>2) <= 0) || ((p>>2) >= OT_LEN) )
		return pol4;

	// Initialize a quad primitive
	setPolyF3( pol4 );

	// Set the projected vertices to the primitive
	gte_stsxy0( &pol4->x0 );
	gte_stsxy1( &pol4->x1 );
	gte_stsxy2( &pol4->x2 );

	// Test if tri is off-screen, discard if so
	if( tri_clip( &screen_clip,
	(DVECTOR*)&pol4->x0, (DVECTOR*)&pol4->x1, 
	(DVECTOR*)&pol4->x2))
		return pol4;

	//const CVECTOR color = color[numMesh];
	setRGB0(pol4, color.r, color.g, color.b);
	// Load primitive color even though gte_ncs() doesn't use it.
	// This is so the GTE will output a color result with the
	// correct primitive code.
	gte_ldrgb( &pol4->r0 );
	
	
	// Load the face normal
	gte_ldv0( normal );
	
	// Normal Color Single
	gte_nccs();
	
	// Store result to the primitive
	gte_strgb( &pol4->r0 );
	
	gte_avsz4();
	gte_stotz( &p );

	
	addPrim( db[db_active].ot+(p>>2), pol4 );
	pol4++;
	return pol4;
}

template<typename T>
static inline POLY_F3* renderMesh(MATRIX* mtx, const T& model, int& p, POLY_F3* pol4, VECTOR& pos, SVECTOR& rot)
{
	return renderMesh(mtx, &model.mesh[0], &model.normals[0], &model.colors[0], &model.sizes[0], model.count, p, pol4, pos, rot);
}

static POLY_F3* renderMesh(MATRIX* mtx, const SVECTOR* mesh, const SVECTOR* normals, const CVECTOR* colors, const int* sizes, int count, int& p, POLY_F3* pol4, VECTOR& pos, SVECTOR& rot)
{
	/* Set rotation and translation to the matrix */
	//SVECTOR rot{0,0,0,0};	
	
	// Object and light matrix for object
	MATRIX omtx,lmtx;
	
	// Set object rotation and position
	RotMatrix( &rot, &omtx );
	TransMatrix( &omtx, &pos );
	
	// Multiply light matrix to object matrix
	MulMatrix0( &light_mtx, &omtx, &lmtx );
	
	// Set result to GTE light matrix
	gte_SetLightMatrix( &lmtx );
	
	// Composite coordinate matrix transform, so object will be rotated and
	// positioned relative to camera matrix (mtx), so it'll appear as
	// world-space relative.
	CompMatrixLV( mtx, &omtx, &omtx );
	
	// Save matrix
	PushMatrix();
	
	// Set matrices
	gte_SetRotMatrix( &omtx );
	gte_SetTransMatrix( &omtx );
	int currentIdx = 0;
	for(int numMesh = 0; numMesh < count; ++numMesh)
	{
		for(int i=0; i < sizes[numMesh];i+=3)
		{
			pol4 = renderTriangle(mesh+currentIdx, normals+(int)currentIdx/3, *(colors+numMesh), pol4);
			currentIdx+=3;
		}
	}
	PopMatrix();
	return pol4;
}

volatile int fps;
volatile int fps_counter;
volatile int fps_measure;

void vsync_cb(void) {
    fps_counter++;
    if( fps_counter >= 60 ) {
        fps = fps_measure;
        fps_measure = 0;
        fps_counter = 0;
    }
}

Map map;

int main() {

	int i,p,xy_temp;
	int px,py;
	
	SVECTOR	rot;			// Rotation vector for cube
	Vector	pos;			// Position vector for cube
	
	SVECTOR verts[33][33];	// Vertex array for floor
	
	//VECTOR	cam_pos;		// Camera position (in fixed point integers)
	//VECTOR	cam_rot;		// Camera view angle (in fixed point integers)
	Camera camera;
	int		cam_mode;		// Camera mode (between first-person and look-at)
	
	VECTOR	tpos;			// Translation value for matrix calculations
	SVECTOR	trot;			// Rotation value for matrix calculations
	MATRIX	lmtx;		// Rotation matrices for geometry and lighting
	
	PADTYPE *pad;			// Pad structure pointer for parsing controller
	
	POLY_F3	*pol4;			// Flat shaded quad primitive pointer
	const auto SEED = 16;
	srand(SEED);
	int rands[SEED];
	for(int x = 0; x < SEED; ++x)
		rands[x] = rand()%16;
	
	// Init graphics and GTE
	init();
	
	
	// Set coordinates to the vertex array for the floor
	for( py=0; py<33; py++ ) {
		for( px=0; px<33; px++ ) { 
			
			setVector( &verts[py][px], 
				(100*(px-8))-50, 
				0, 
				(100*(py-8))-50 );
			
		}
	}
	
	camera.SetPosition(0,-210,-310);
	camera.SetRotation(208,-208,0);
	// Camera default coordinates
	//setVector( &cam_pos, ONE*-366, ONE*-1062, ONE*588 );
	//setVector( &cam_rot, ONE*784, ONE*-1816, 0 );
	
	VSyncCallback(vsync_cb);
	// Main loop
	VECTOR charPos{0,0,0};
	SVECTOR charRot{0,0,0,0};
	int posx = 0;
	int posz = 0;
	long mapIdxX = 0;
	long mapIdxZ = 0;
	while( 1 ) {
		
		// Set pad buffer data to pad pointer
		pad = (PADTYPE*)&pad_buff[0][0];
		
		// Parse controller input
		cam_mode = 0;

		if( pad->stat == 0 ) {
			
			// For digital pad, dual-analog and dual-shock
			if( ( pad->type == 0x4 ) || ( pad->type == 0x5 ) || ( pad->type == 0x7 ) ) {
				
				// The button status bits are inverted,
				// so 0 means pressed in this case
				
				// Look controls
				if( !(pad->btn&PAD_UP) ) {
					camera.Roll(8);
				} else if( !(pad->btn&PAD_DOWN) ) {
					camera.Roll(-8);
				}
				
				if( !(pad->btn&PAD_LEFT) ) {
					camera.Pich(8);
				} else if( !(pad->btn&PAD_RIGHT) ) {
					camera.Pich(-8);
				}
				
				// Movement controls
				if( !(pad->btn&PAD_TRIANGLE) ) {
					//camera.Translate(camera.Forward() * 5);
					posz+=5;
					mapIdxZ+=5;
				} else if( !(pad->btn&PAD_CROSS) ) {
					//camera.Translate(camera.Forward() * -5);
					mapIdxZ-=5;
					posz-=5;
				}
				
				if( !(pad->btn&PAD_SQUARE ) ) {
					//camera.Translate(Vector{-50000,0,0});
					mapIdxX+=5;
					posx+=5;
				} else if( !(pad->btn&PAD_CIRCLE ) ) {
					mapIdxX-=5;
					posx-=5;
					//camera.Translate(Vector{50000,0,0});
				}
				
				/*posx = posx > 100 ? 0 : posx;
				posx = posx < 0 ? 100 : posx;

				posz = posz > 100 ? 0 : posz;
				posz = posz < 0 ? 100 : posz;*/
				

				posx = posx >= map.SCALE ? posx - map.SCALE : posx;
				posx = posx < 0 ? posx + map.SCALE : posx;

				posz = posz >= map.SCALE ? posz - map.SCALE : posz;
				posz = posz < 0 ? posz + map.SCALE : posz;
				//posz = posz%100;
				
				if( !(pad->btn&PAD_R1) ) {
					camera.Translate(camera.Up() * 5);
				}
				
				if( !(pad->btn&PAD_R2) ) {
					camera.Translate(camera.Up() * -5);
				}
				
				// Look at cube
				if( !(pad->btn&PAD_L1) ) {
					//cam_mode = 1;
				}
				
				if( !(pad->btn&PAD_SELECT) ) {
					_boot();
				}
				
			}
			
			// For dual-analog and dual-shock (analog input)
			if( ( pad->type == 0x5 ) || ( pad->type == 0x7 ) ) {
				
				// Moving forwards and backwards
				if( ( (pad->ls_y-128) < -16 ) || ( (pad->ls_y-128) > 16 ) ) {
					camera.Translate(camera.Forward() * (((pad->ls_y-128))>>5));
				}
				
				// Strafing left and right
				if( ( (pad->ls_x-128) < -16 ) || ( (pad->ls_x-128) > 16 ) ) {
					camera.Translate(camera.Right() * (((pad->ls_x-128))>>5));
				}
				
				// Look up and down
				if( ( (pad->rs_y-128) < -16 ) || ( (pad->rs_y-128) > 16 ) ) {
					camera.Roll((pad->rs_y-128)<<9);
				}
				
				// Look left and right
				if( ( (pad->rs_x-128) < -16 ) || ( (pad->rs_x-128) > 16 ) ) {
					camera.Pich((pad->rs_x-128)<<9);
				}
				
			}
		
		}
		
		// Print out some info
		FntPrint(-1, "BUTTONS=%04x\n", pad->btn);
		FntPrint(-1, "X=%d Y=%d Z=%d\n", 
			mapIdxX, 
			camera.Position().vy, 
			mapIdxZ);
		FntPrint(-1, "RX=%d RY=%d\n", 
			camera.Rotation().vx, 
			camera.Rotation().vy);
		
		// First-person camera mode
		if( cam_mode == 0 ) {
			camera.Update();
		// Tracking mode
		} else {
			camera.LookAt(pos);
		}
		MATRIX mtx = camera.Matrix();	
		
		// Draw the floor
		pol4 = (POLY_F3*)db_nextpri;

		const auto position = camera.Position().vx/2000.0f;
		FntPrint(-1, "IDX=%d\n", camera.Position().vx);

		FntPrint(-1, "FPS=%d\n", fps);

		character1 char1;
		VECTOR charPos = camera.Position();
		charPos.vx += 256;
		charPos.vy = 0;
		charPos.vz = 430;

		charRot.vy = -1024;
	    pol4 = renderMesh(&mtx, &char1.mesh[0], &char1.normals[0], &char1.colors[0], &char1.sizes[0], char1.count, p, pol4, charPos, charRot);
		
		/*for(int i = position; i< position+3; ++i)
		{
			const int idx = rands[i%SEED]%3;
			
			VECTOR pos{(i)*2000+300,0,400};
			SVECTOR rot{0,0,0,0};
			//pol4 = renderMesh(&mtx, mesh[idx], normal[idx], *color[idx], size[idx], p, pol4, pos, rot);
			//pol4 = renderMesh(&mtx, grass1, p, pol4, pos, rot);
		}*/
		
		


		const auto& mapMesh = map.GenerateMesh(mapIdxX, mapIdxZ);
		
		VECTOR mapPos { posx + map.SCALE*12, 0, -posz - map.SCALE*12};
		
		pol4 = renderMesh(&mtx, &mapMesh.mesh[0], &mapMesh.normals[0], &mapMesh.colors[0], &mapMesh.sizes[0], mapMesh.count, p, pol4, mapPos, charRot);
		fps_measure++;
		
		// Update nextpri variable (very important)
		db_nextpri = (char*)pol4;
		
		
		// Position the cube going around the floor bouncily
		setVector( &pos, 
			isin( rot.vy )>>4, 
			-300+(isin( rot.vy<<2 )>>5), 
			icos( rot.vy )>>3 );
		
		// Sort cube
		//sort_cube( &mtx, &pos, &rot );
		
		// Make the cube SPEEN
		rot.vx += 8;
		rot.vy += 8;
		
		
		// Flush text to drawing area
		FntFlush(-1);
	
		// Swap buffers and draw the primitives
		display();
		
	}
	
	return 0;
	
}

void sort_cube(MATRIX *mtx, VECTOR *pos, SVECTOR *rot) {

	int i,p;
	POLY_F4 *pol4;
	
	// Object and light matrix for object
	MATRIX omtx,lmtx;
	
	// Set object rotation and position
	RotMatrix( rot, &omtx );
	TransMatrix( &omtx, pos );
	
	// Multiply light matrix to object matrix
	MulMatrix0( &light_mtx, &omtx, &lmtx );
	
	// Set result to GTE light matrix
	gte_SetLightMatrix( &lmtx );
	
	// Composite coordinate matrix transform, so object will be rotated and
	// positioned relative to camera matrix (mtx), so it'll appear as
	// world-space relative.
	CompMatrixLV( mtx, &omtx, &omtx );
	
	// Save matrix
	PushMatrix();
	
	// Set matrices
	gte_SetRotMatrix( &omtx );
	gte_SetTransMatrix( &omtx );
	
	// Sort the cube
	pol4 = (POLY_F4*)db_nextpri;
	
	for( i=0; i<CUBE_FACES; i++ ) {
		
		// Load the first 3 vertices of a quad to the GTE 
		gte_ldv3( 
			&cube_verts[cube_indices[i].v0], 
			&cube_verts[cube_indices[i].v1], 
			&cube_verts[cube_indices[i].v2] );
			
		// Rotation, Translation and Perspective Triple
		gte_rtpt();
		
		// Compute normal clip for backface culling
		gte_nclip();
		
		// Get result
		gte_stopz( &p );
		
		// Skip this face if backfaced
		if( p < 0 )
			continue;
		
		// Calculate average Z for depth sorting
		gte_avsz3();
		gte_stotz( &p );
		
		// Skip if clipping off
		// (the shift right operator is to scale the depth precision)
		if( ((p>>2) <= 0) || ((p>>2) >= OT_LEN) )
			continue;
		
		// Initialize a quad primitive
		setPolyF4( pol4 );
		
		// Set the projected vertices to the primitive
		gte_stsxy0( &pol4->x0 );
		gte_stsxy1( &pol4->x1 );
		gte_stsxy2( &pol4->x2 );
		
		// Compute the last vertex and set the result
		gte_ldv0( &cube_verts[cube_indices[i].v3] );
		gte_rtps();
		gte_stsxy( &pol4->x3 );
		
		// Test if quad is off-screen, discard if so
		if( quad_clip( &screen_clip,
		(DVECTOR*)&pol4->x0, (DVECTOR*)&pol4->x1, 
		(DVECTOR*)&pol4->x2, (DVECTOR*)&pol4->x3 ) )
			continue;
		
		// Load primitive color even though gte_ncs() doesn't use it.
		// This is so the GTE will output a color result with the
		// correct primitive code.
		gte_ldrgb( &pol4->r0 );
		
		// Load the face normal
		gte_ldv0( &cube_norms[i] );
		
		// Normal Color Single
		gte_ncs();
		
		// Store result to the primitive
		gte_strgb( &pol4->r0 );
		
		gte_avsz4();
		gte_stotz( &p );
		
		// Sort primitive to the ordering table
		addPrim( db[db_active].ot+(p>>2), pol4 );
		
		// Advance to make another primitive
		pol4++;
		
	}
	
	// Update nextpri
	db_nextpri = (char*)pol4;
	
	// Restore matrix
	PopMatrix();
	
}

void init() {

	// Reset the GPU, also installs a VSync event handler
	ResetGraph( 0 );

	// Set display and draw environment areas
	// (display and draw areas must be separate, otherwise hello flicker)
	SetDefDispEnv( &db[0].disp, 0, 0, SCREEN_XRES, SCREEN_YRES );
	SetDefDrawEnv( &db[0].draw, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES );
	
	// Enable draw area clear and dither processing
	setRGB0( &db[0].draw, 0,0,0 );
	db[0].draw.isbg = 1;
	db[0].draw.dtd = 1;
	
	
	// Define the second set of display/draw environments
	SetDefDispEnv( &db[1].disp, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES );
	SetDefDrawEnv( &db[1].draw, 0, 0, SCREEN_XRES, SCREEN_YRES );
	
	setRGB0( &db[1].draw, 0,0,0 );
	db[1].draw.isbg = 1;
	db[1].draw.dtd = 1;
	
	// Apply the drawing environment of the first double buffer
	PutDrawEnv( &db[0].draw );
	
	// Clear both ordering tables to make sure they are clean at the start
	ClearOTagR( db[0].ot, OT_LEN );
	ClearOTagR( db[1].ot, OT_LEN );
	
	// Set primitive pointer address
	db_nextpri = db[0].p;
	
	// Set clip region
	setRECT( &screen_clip, 0, 0, SCREEN_XRES, SCREEN_YRES );
	
	
	// Initialize the GTE
	InitGeom();
	
	// Set GTE offset (recommended method  of centering)
	gte_SetGeomOffset( CENTERX, CENTERY );
	
	// Set screen depth (basically FOV control, W/2 works best)
	gte_SetGeomScreen( CENTERX );
	
	// Set light ambient color and light color matrix
	gte_SetBackColor( 63, 63, 63 );
	gte_SetColorMatrix( &color_mtx );
	
	
	// Init BIOS pad driver and set pad buffers (buffers are updated
	// automatically on every V-Blank)
	InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);
	
	// Start pad
	StartPAD();
	
	// Don't make pad driver acknowledge V-Blank IRQ (recommended)
	ChangeClearPAD(0);
	
	// Load font and open a text stream
	FntLoad(960, 0);
	FntOpen(0, 8, 320, 216, 0, 100);
	
}

void display() {
	
	// Wait for GPU to finish drawing and vertical retrace
	DrawSync( 0 );
	VSync( 0 );
	
	// Swap buffers
	db_active ^= 1;
	db_nextpri = db[db_active].p;
	
	// Clear the OT of the next frame
	ClearOTagR( db[db_active].ot, OT_LEN );
	
	// Apply display/drawing environments
	PutDrawEnv( &db[db_active].draw );
	PutDispEnv( &db[db_active].disp );
	
	// Enable display
	SetDispMask( 1 );
	
	// Start drawing the OT of the last buffer
	DrawOTag( db[1-db_active].ot+(OT_LEN-1) );
}

