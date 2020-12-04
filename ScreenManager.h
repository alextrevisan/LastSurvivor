#ifndef _SCREEN_MANAGER_H_
#define _SCREEN_MANAGER_H_

#include <psxgpu.h>

namespace {
// OT and Packet Buffer sizes
static constexpr unsigned short OT_LEN = 1024;
static constexpr unsigned short PACKET_LEN = 32768;

// Screen resolution
static constexpr unsigned short SCREEN_XRES = 320;
static constexpr unsigned short SCREEN_YRES = 240;

// Screen center position
static constexpr unsigned short CENTERX	= SCREEN_XRES>>1;
static constexpr unsigned short CENTERY	= SCREEN_YRES>>1;
}

class ScreenManager
{
    public:
    ScreenManager(bool loadFont);
    ~ScreenManager();

    static inline void AddPrimirive(POLY_FT3* polygon);

private:
    // Double buffer structure
    typedef struct {
        DISPENV	disp;			// Display environment
        DRAWENV	draw;			// Drawing environment
        unsigned int ot[OT_LEN];// Ordering table
        char p[PACKET_LEN];     // Packet buffer
    } DB;

    // Double buffer variables
    static DB db[2];
    static int db_active;
    static char *db_nextpri;
    static RECT screen_clip;
};


#endif //_SCREEN_MANAGER_H_