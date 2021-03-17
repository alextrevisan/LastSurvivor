#ifndef _MAIN_MENU_H_
#define _MAIN_MENU_H_
#include <TextureManager.h>
#include <Graphics.h>

extern unsigned int parchment_tim[];
TIM_IMAGE parchment_texture;

extern unsigned int start_tim[];
TIM_IMAGE start_texture;


struct MainMenu
{
    short switchColorCount = 0;
    MainMenu()
    {
        TextureManager::LoadTexture(parchment_tim, parchment_texture);
        TextureManager::LoadTexture(start_tim, start_texture);
    }
    template<typename T>
    void RenderMainUI(T* graphics)
    {
        
        const auto startposx = (graphics->Width>>1) - (64>>1);
        const auto startposy = (graphics->Height>>1) - (32>>1);

        const auto bgposx = (graphics->Width>>1) - (256>>1);
        const auto bgposy = (graphics->Height>>1) - (192>>1);

        switchColorCount++;
        if(switchColorCount<30) //1sec for displaying each color
        {
            graphics->DrawSpriteRect(start_texture, {startposx,startposy,64,32});
        }
        else if(switchColorCount<64)
        {
            graphics->DrawSpriteRect(start_texture, {startposx,startposy,64,32}, {64,0});
        }
        else
        {
            switchColorCount=0;
        }
        graphics->DrawSprite(parchment_texture, {bgposx,bgposy,0});
    }
};


#endif //_MAIN_MENU_H_