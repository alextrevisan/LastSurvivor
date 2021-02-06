#ifndef _TEXTURE_MANAGER_H_
#define _TEXTURE_MANAGER_H_

#include <psxgpu.h>

class TextureManager
{
public:
    static void LoadTexture(unsigned int* bytes, TIM_IMAGE &texture)
    {
        GetTimInfo(bytes, &texture);
	
        LoadImage(texture.prect, texture.paddr);
        DrawSync(0);
        
        LoadImage(texture.crect, texture.caddr);
        DrawSync(0);    
    }
};



#endif //_TEXTURE_MANAGER_H_