#include <ScreenManager.h>
#include <psxgte.h>
#include <psxgpu.h>
#include <inline_c.h>

ScreenManager::ScreenManager(bool loadFont)
{
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
	//gte_SetColorMatrix( &color_mtx );
	
	if(loadFont)
    {
        // Load font and open a text stream
        FntLoad(960, 0);
        FntOpen(0, 8, 320, 216, 0, 100);
    }
}

inline void ScreenManager::AddPrimirive(POLY_FT3* polygon)
{
    int p;
    addPrim(db[db_active].ot+(p>>2), polygon );
}

ScreenManager::~ScreenManager()
{
}