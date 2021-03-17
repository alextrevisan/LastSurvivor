#ifndef _HUD_H_
#define _HUD_H_
#include <TextureManager.h>
#include <Graphics.h>

extern unsigned int compass_tim[];
extern unsigned int compass_container_tim[];
TIM_IMAGE compass_texture;
TIM_IMAGE compass_container_texture;

struct HUD
{
    HUD()
    {
        TextureManager::LoadTexture(compass_tim, compass_texture);
        TextureManager::LoadTexture(compass_container_tim, compass_container_texture);
    }

    template<typename T>
    void RenderHUD(T* graphics, int cameraRotation)
    {
        const auto startposx = (graphics->Width>>1) - (64>>1);
        const auto startposy = 4;
        graphics->DrawSpriteRect(compass_container_texture, {startposx-8,startposy,80, 28}, {0,0});
        graphics->DrawSpriteRect(compass_texture, {startposx,startposy+6,64,16}, {(-cameraRotation+512)>>4,0});
        
    }
};


#endif //_HUD_H_