#ifndef _AMBIENT_LIGHT_CONTROLLER_H_
#define _AMBIENT_LIGHT_CONTROLLER_H_

#include <psxgte.h>
//https://www.reddit.com/r/gamedev/comments/7cti1q/2d_daynight_cycles/
enum TimeOfDay {
    SUN_SUNSET_START = 796,
    SUN_SUNSET_SWITCH = 910,
    SUN_SUNSET_END = 989,

    SUN_SUNRISE_START = 3163,
    SUN_SUNRISE_SWITCH = 3197,
    SUN_SUNRISE_END = 3265,
};

struct AmbientLightController
{
    
    struct math
    {
        static inline int lerp( const int min, const int max, const int steps, const int current)
        {
            if (steps == 0) return max;
            return min + ((((int)(max - min)<<12)/steps * current)>>12);
        }
    };

    static inline int TimeLerp (int time, int start, int end)
    {
        return math::lerp(start, end, end - start, time - start) - start;
    }

    static inline const CVECTOR BlendColour (const CVECTOR& start, const CVECTOR& end, int steps, int lerp)
    {
        unsigned char r = math::lerp(start.r, end.r, steps, lerp);
        unsigned char g = math::lerp(start.g, end.g, steps, lerp);
        unsigned char b = math::lerp(start.b, end.b, steps, lerp);
        return {r,g,b};
    }

    static constexpr int steps = 4096;
    static CVECTOR color;
    static inline CVECTOR executeWithTime(short time)
    {
        
        if (time >= SUN_SUNSET_START && time <= SUN_SUNRISE_END)
        {
            if (time <= SUN_SUNSET_END)
            {
                if (time <= SUN_SUNSET_SWITCH)
                {
                    // Day time colour start to orange end
                    const int timeLerp = TimeLerp (time, SUN_SUNSET_START, SUN_SUNSET_SWITCH);
                    constexpr CVECTOR start{191>>1,182>>1,127>>1};//255,255,255
                    constexpr CVECTOR end{127>>1,102>>1, 38>>1};            
                    color = BlendColour(start, end, SUN_SUNSET_SWITCH - SUN_SUNSET_START, timeLerp);
                    return color;
                    //{255,255,255}, {127,102, 38}, {time,time,time}
                    //char red = math::lerp<int, 255, 102, 4096>(time);
                }
                else
                {
                    // Orange start to night time colour end
                    const int timeLerp = TimeLerp (time, SUN_SUNSET_SWITCH, SUN_SUNSET_END);
                    constexpr CVECTOR start{127>>1,102>>1, 38>>1};
                    constexpr CVECTOR end{14,8,43};//25,102,229
                    color = BlendColour(start, end, SUN_SUNSET_END - SUN_SUNSET_SWITCH, timeLerp);
                    return color;
                }
            }
            else if (time >= SUN_SUNRISE_START)
            {
                if (time <= SUN_SUNRISE_SWITCH)
                {  
                    // Night time colour start to pink/purple end
                    const int timeLerp = TimeLerp (time, SUN_SUNRISE_START, SUN_SUNRISE_SWITCH);
                    //25 102 229
                    constexpr CVECTOR start{14,8,43};
                    constexpr CVECTOR end{127>>1,127>>1,127>>1};            
                    color = BlendColour(start, end, SUN_SUNRISE_SWITCH - SUN_SUNRISE_START, timeLerp);
                    return color;
                }
                else
                {
                    // Pink/purple start to day time colour end
                    const int timeLerp = TimeLerp (time, SUN_SUNRISE_SWITCH, SUN_SUNRISE_END);
                    constexpr CVECTOR start{127>>1, 127>>1, 127>>1};
                    constexpr CVECTOR end{191>>1,182>>1,127>>1};//255,255,255
                    color = BlendColour(start, end, SUN_SUNRISE_END - SUN_SUNRISE_SWITCH, timeLerp);
                    return color;
                }
            }
            else
            {
                // Night time //25 102 229
                return {14,8,43};
            }
        }
        else
        {
            // Day time //255,255,255
            return {191>>1,182>>1,127>>1};
        }
        return color;
    }
};

CVECTOR AmbientLightController::color = {0};
#endif //_AMBIENT_LIGHT_CONTROLLER_H_