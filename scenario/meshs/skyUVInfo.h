#ifndef _SKY_UV_INFO_H_
#define _SKY_UV_INFO_H_

#include <psxgte.h>

namespace {
    short abs(short value)
    {
        return value > 0 ? value : -value;
    }
}

struct skyUVOffset
{
    //16, 42
    //16, 105
    const DVECTOR offset[3] = {
        {0, 0},
        {0, 63},
        {0, -63},
    };
    const int offsetDurations[3] = {
        0,
        2048,
        2048,
        
    };
    int currentTimer = 0;
    unsigned char currentDayCycle = 2;
    //bigger the number, slower
    static constexpr int skyspeed = 0;//11 = 341 seconds ~5.7 minutes (day/if offset == 10)
                                       //10 = 170 seconds ~2.8 minutes (day/if offset == 10)
                                       //9  = 85 seconds  ~1.4 minutes (day/if offset == 10)
    
    const DVECTOR getValueToAdd()
    {
        static int currentOffsetDuration = 0;
        static int ticksToChangeU = 0;
        static int ticksToChangeV = 0;
        if(currentOffsetDuration==0)
        {
            currentDayCycle++;
            if(currentDayCycle==3)
                currentDayCycle = 0;
            currentOffsetDuration = offsetDurations[currentDayCycle]<<skyspeed;
            
            if(currentDayCycle == 0)
                return offset[0];

            ticksToChangeU = getTicksToChangeU();
            ticksToChangeV = getTicksToChangeV();
        }
        currentOffsetDuration--;

        DVECTOR ret{0,0};
        if(ticksToChangeU-- == 0)
        {
            ret.vx = offset[currentDayCycle].vx > 0 ? 1 : -1;
            ticksToChangeU = getTicksToChangeU();
        }
            
        if(ticksToChangeV-- == 0)
        {
            ret.vy = offset[currentDayCycle].vy > 0 ? 1 : -1;
            ticksToChangeV = getTicksToChangeV();
        }

        return ret;
    }

    inline int getTicksToChangeU() const
    {
        if(offset[currentDayCycle].vx == 0)
            return (offsetDurations[currentDayCycle]<<skyspeed)+2;

        return ((offsetDurations[currentDayCycle]<<skyspeed) / abs(offset[currentDayCycle].vx));
    }

    

    inline int getTicksToChangeV() const
    {
        if(offset[currentDayCycle].vy == 0)
            return (offsetDurations[currentDayCycle]<<skyspeed)+2;

        return ((offsetDurations[currentDayCycle]<<skyspeed) / abs(offset[currentDayCycle].vy));
    }

};
/*
day -> 114, 76
starting night -> 114, 134
night -> 20, 134
starting day -> { 14, 218 },
day -> 116, 215
*/


#endif //_SKY_UV_INFO_H_
