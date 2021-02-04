#ifndef _JOYSTICK_H_
#define _JOYSTICK_H_

#include <psxpad.h>
#include <psxapi.h>

struct Joystick
{
    Joystick()
    {
        // Init BIOS pad driver and set pad buffers (buffers are updated
	    // automatically on every V-Blank)
	    InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);
        // Start pad
        StartPAD();
        // Don't make pad driver acknowledge V-Blank IRQ (recommended)
        ChangeClearPAD(0);
    }

    inline bool IsPressed(const unsigned short button) const
    {
        // The button status bits are inverted,
        // so 0 means pressed in this case
        return !(pad->btn&button);
    }

    inline unsigned char Status()
    {
        return pad->stat;
    }

    inline char LeftStickX()
    {
        return pad->ls_x;
    }

    inline char LeftStickY()
    {
        return pad->ls_y;
    }

    inline char RightStickX()
    {
        return pad->rs_x;
    }

    inline char RightStickY()
    {
        return pad->rs_y;
    }

    inline bool IsDigital() const
    {
        return pad->type == 0x4;
    }

    inline bool IsAnalog() const 
    {
        return pad->type == 0x5;
    }

    inline bool IsDualshock() const 
    {
        return pad->type == 0x7;
    }

    inline bool IsDualAnalog()
    {
        return IsDualshock();
    }

    char pad_buff[2][34];
    PADTYPE const* pad = (PADTYPE*)&pad_buff[0][0];
};


#endif //_JOYSTICK_H_