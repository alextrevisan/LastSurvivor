#include <stdio.h>
#include <string.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <inline_c.h>
#include <Graphics.h>
#include <Joystick.h>
#include <TextureManager.h>
#include <animated/cube.h>
#include <camera.h>
#include <Util.h>
#include <scenario/meshs/skysphere.h>
#include <scenario/meshs/skyUVInfo.h>
#include <scenario/meshs/hills.h>
#include<scenario/meshs/sun.h>


#include <scenario/Map.h>

#include <AmbientLightController.h>


#include <NoiseGenerator.h>
#include <PerlinNoise.h>
#include <MathUtils.h>

#include <gamestate/GameState.h>

#include <ui/MainMenu.h>
#include <ui/HUD.h>

/* Number of faces of cube */
#define CUBE_FACES 6

template<typename T>
T abs(const T& value)
{
    return value > 0 ? value : -value;
}
/* Light color matrix */
/* Each column represents the color matrix of each light source and is */
/* used as material color when using gte_ncs() or multiplied by a */
/* source color when using gte_nccs(). 4096 is 1.0 in this matrix */
/* A column of zeroes disables the light source. */
MATRIX color_mtx = {
    4096, 0, 0,	/* Red   */
    4096, 0, 0,	/* Green */
    4096, 0, 0	/* Blue  */
};

/* Light matrix */
/* Each row represents a vector direction of each light source. */
/* An entire row of zeroes disables the light source. */
MATRIX light_mtx = {
    /* X,  Y,  Z */
    -2048 , -512 , -2048,
    //0,0,0,
    0	  , 0	  , 0,
    0	  , 0	  , 0
};/*141,157,52
151,167,244*/

extern unsigned int sunset0_tim[];
extern unsigned int sunset1_tim[];
extern unsigned int sunset2_tim[];
extern unsigned int sunset3_tim[];
extern unsigned int sunset4_tim[];

extern unsigned int skysphere_tim[];
extern unsigned int hills_tim[];
extern unsigned int sun_tim[];

/*TIM_IMAGE sunset0_texture;
TIM_IMAGE sunset1_texture;
TIM_IMAGE sunset2_texture;
TIM_IMAGE sunset3_texture;
TIM_IMAGE sunset4_texture;*/
TIM_IMAGE skysphere_texture;

TIM_IMAGE hills_texture;
TIM_IMAGE sun_texture;

GraphClass *graphics;

MATRIX DrawSkyBox(const MATRIX& mtx);
MATRIX DrawSun(const MATRIX& mtx);

int main() {
    graphics = new GraphClass(2048,65536);
    int i,p,xy_temp;
    
    SVECTOR	rot = { 0 };			/* Rotation vector for Rotmatrix */
    VECTOR	pos = { 0, 0, 0 };	/* Translation vector for TransMatrix */
    
        
    
    /* Init graphics and GTE */
    graphics->SetResolution(320,240);
    //gte_SetColorMatrix( &color_mtx );

    FntLoad(960, 0);
    FntOpen(0, 8, 320, 216, 0, 100);
    DrawSync(0);
    
    
    TextureManager::LoadTexture(skysphere_tim, skysphere_texture);
    TextureManager::LoadTexture(hills_tim, hills_texture);
    TextureManager::LoadTexture(sun_tim, sun_texture);
    
    

    Joystick joystick = Joystick();

    Camera camera;
    // Object and light matrix for object
    
    
    camera.LookAt({0,0,0});

    //camera.SetPosition(12457346, 3932160 , 27444391);
    // Multiply light matrix to object matrix
    
    /* Main loop */
    //camera.SetPosition(686592, -2445312, 11012608);
    //camera.SetPosition(0, -(352<<12), 0);
    camera.SetPosition((11<<12)*512,-768<<12,(11<<12)*512);
    

    int mapPosX=(0<<12), mapPosZ=(0<<12);
    auto *map = new Map(graphics, mapPosX, mapPosZ);


    MATRIX objectMTX{0},lightMTX{0};

    MainMenu mainMenu;
    GameState gameState;
    
    TextureManager::LoadTexture(parchment_tim, parchment_texture);
    while( 1 ) {
        
        
        //FntPrint(-1, "x,y = %d,%d", parchment_texture.prect->x, parchment_texture.prect->y);
        
        
        if(joystick.Status() == 0)
        {
            if( joystick.IsAnalog() || joystick.IsDualshock() )
            {
                const char LSX = joystick.LeftStickX() - 128;
                const char LSY = joystick.LeftStickY() - 128;
                const char RSX = joystick.RightStickX() - 128;
                const char RSY = joystick.RightStickY() - 128;

                if(RSY < -24)
                {
                    camera.Roll(RSY*4);
                }
                else if(RSY > 24)
                {
                    camera.Roll(RSY*4);
                }

                if(RSX < -24)
                {
                    camera.Pich(-RSX*4);
                    
                }
                else if(RSX > 24)
                {
                    camera.Pich(-RSX*4);
                }

                if(joystick.IsPressed(PAD_LEFT))
                {
                    //camera.Translate(-ONE,0,0);
                    //rot.vy += 8;
                }
                if(joystick.IsPressed(PAD_RIGHT))
                {
                    //camera.Translate(ONE,0,0);
                    //rot.vy -= 8;
                }

                if(joystick.IsPressed(PAD_DOWN))
                {
                    //camera.Translate(0,ONE*4,0);
                }
                if(joystick.IsPressed(PAD_UP))
                {
                    //camera.Translate(0,-ONE*4,0);
                }

                if(joystick.JustPressed(PAD_START))
                {
                    gameState.SetGameState((gameState.GetCurrentGameState() == GameStates::MainMenu ? GameStates::Level01 : GameStates::MainMenu));
                }

                static int count = 0;
                const int FPS = graphics->FPS();
                if(gameState.GetCurrentGameState() == GameStates::Level01 && FPS > 0)
                {
                    static HUD hud;
                    hud.RenderHUD(graphics, camera.Rotation().vy);
                    
                    auto diff = 240;
                    if(FPS >= 30);
                        diff = diff/FPS;

                    const int sin = isin(camera.Rotation().vy);
                    const int cos = icos(camera.Rotation().vy);
                    const auto lsysin = (LSY*sin*(diff>>1));
                    const auto lsxcos = (LSX*cos*(diff>>1));
                    const auto lsycos = (LSY*cos*(diff>>1));
                    const auto lsxsin = (LSX*sin*(diff>>1));
                    
                    //if(LSX > 32 || LSX < -32 || LSY > 32 || LSY < -32)
                    {
                        mapPosX += (lsysin + lsxcos)>>12;
                        mapPosZ += (-lsycos + lsxsin)>>12;
                    }
                }
                
            }            
        }

        
        FntPrint(-1, "FPS=%d\n", graphics->FPS());

        camera.Update();//Set rotation and transformation matrix;
        
        const auto& cameraMTX = camera.Matrix();

        RotMatrix( &rot, &objectMTX );
        TransMatrix( &objectMTX, &pos );
        
        
        // Composite coordinate matrix transform, so object will be rotated and
        // positioned relative to camera matrix (mtx), so it'll appear as
        // world-space relative.
        CompMatrixLV( &cameraMTX, &objectMTX, &objectMTX );
        //MulMatrix0( &light_mtx, &objectMTX, &lightMTX );

        //gte_SetLightMatrix( &lightMTX );
        gte_SetRotMatrix( &objectMTX );
        gte_SetTransMatrix( &objectMTX );
        
        if(gameState.GetCurrentGameState() == GameStates::MainMenu)
        {
            mainMenu.RenderMainUI(graphics);
        }
        else if(gameState.GetCurrentGameState() == GameStates::Level01)
        {
            const auto sunMTX = DrawSkyBox(cameraMTX);
            
            const auto targetY = map->Render(mapPosX, mapPosZ, objectMTX, sunMTX);
            const auto camy = camera.Position().vy;
            constexpr auto gravity = 24;
            if((camy>>12) - targetY > gravity)
                camera.Translate(0,-gravity<<12,0);
            if((camy>>12) - targetY < -gravity)
                camera.Translate(0,gravity<<12,0);
        }

        FntFlush(-1);		
        
        joystick.Update();
        graphics->Display<true>();
        
    }
    
    return 0;
    
}

skysphere sky{.texture = &skysphere_texture};
hills hill{.texture = &hills_texture};

skyUVOffset skyDayOffset;
int skyTickTimer = 3163;
int r = 191,g = 182,b = 127;

MATRIX DrawSkyBox(const MATRIX& mtx)
{
    MATRIX skybox_color_mtx = {
        0, 0, 0,	/* Red   */
        0, 0, 0,	/* Green */
        0, 0, 0	/* Blue  */
    };

    MATRIX skybox_light_mtx = {
        /* X,  Y,  Z */
        0 , 0 , 0,
        0  , 0, 0,
        0	  , 0	  , 0
    };

    MATRIX omtx{0};
    VECTOR pos{0};
    TransMatrix( &omtx, &pos );
    gte_SetColorMatrix(&skybox_color_mtx);
    gte_SetLightMatrix( &skybox_light_mtx );

    /*skyTickTimer++;
    if(skyTickTimer > 10)
    {
        skyTickTimer = 0;
        //gte_SetBackColor(r++,g++,b++);
        if(r > 255) r = 0;
        if(g > 255) g = 0;
        if(b > 255) b = 0;
    }*/

    const auto toAdd = skyDayOffset.getValueToAdd();
    for(int i =0; i < 26; ++i)
    {
        sky.uvs[i].vx += toAdd.vx;
        sky.uvs[i].vy += toAdd.vy;
    }

    skyTickTimer++;
    if(skyTickTimer>(4096))
        skyTickTimer = 0;
                                                              //<<8 = ~30 min complete day cicle
    const auto color = AmbientLightController::executeWithTime(skyTickTimer);

    gte_SetBackColor(color.r, color.g, color.b);

    //FntPrint(-1, "time=%d\n", skyTickTimer	);
    //FntPrint(-1, "rgb=%d,%d,%d\n", color.r, color.g, color.b);
    //FntPrint(-1, "uv=%d,%d\n", sky.uvs[0].vx, sky.uvs[0].vy);
    //FntPrint(-1, "offset=%d\n", skyDayOffset.);
    
    
    
    PushMatrix();
        gte_SetTransMatrix( &omtx );
        graphics->DrawObject3D<skysphere>(sky);
        graphics->DrawObject3D<hills>(hill);
        const auto sunMTX = DrawSun(mtx);
    PopMatrix();
    return sunMTX;
}

SVECTOR sunRotation{0 , -600, 0};

MATRIX DrawSun(const MATRIX& mtx)
{
    constexpr sun mysun{.texture = &sun_texture};
    sunRotation.vz--;
    if(sunRotation.vz < -4096)
        sunRotation.vz = 0;

    MATRIX rmtx{0};
    RotMatrix(&sunRotation, &rmtx); 
    MulMatrix0(&mtx, &rmtx, &rmtx);

    gte_SetRotMatrix( &rmtx );
    graphics->DrawObject3D<sun>(mysun);
    return rmtx;
}