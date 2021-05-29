#ifndef _MAP_H_
#define _MAP_H_

#include <psxgpu.h>

#include <Math.h>
#include <MathUtils.h>

#include <Graphics.h>
#include <TextureManager.h>
#include <AmbientLightController.h>

#include <PerlinNoise.h>
#include <NoiseGenerator.h>

#include <meshs/dead_tree.h>
#include <meshs/grass.h>
#include <meshs/grass_tile.h>

extern unsigned int dead_tree128_tim[];
extern unsigned int grass01_tim[];
extern unsigned int grass_tile01_tim[];

TIM_IMAGE deadtree_texture;
TIM_IMAGE grass01_texture;
TIM_IMAGE grass_tile01_texture;

class Map
{
private:
    static constexpr int MAP_SIZE = 32;
    static constexpr int CENTER_X = MAP_SIZE >> 1;
    static constexpr int CENTER_Z = MAP_SIZE >> 1;

    GraphClass *Graphics;

    int LastPosX{0};
    int LastPosZ{0};

    int map[MAP_SIZE][MAP_SIZE]{0};
    unsigned char decorations[MAP_SIZE][MAP_SIZE]{0};

public:
    Map(GraphClass *graphics, int mapPosX, int mapPosZ)
        : Graphics(graphics), LastPosX(mapPosX), LastPosZ(mapPosZ)
    {
        TextureManager::LoadTexture(dead_tree128_tim, deadtree_texture);
        TextureManager::LoadTexture(grass01_tim, grass01_texture);
        TextureManager::LoadTexture(grass_tile01_tim, grass_tile01_texture);
        GenerateFullMap(mapPosZ, mapPosZ);
    }

    inline void RenderDecorations(const int noise, const SVECTOR& position) const
    {
        constexpr dead_tree deadtree01{.texture = &deadtree_texture};
        constexpr grass grass01{.texture = &grass01_texture};

        constexpr unsigned char TREE_CHANCE = 78;
        constexpr unsigned char GRASS_CHANCE = 79;

        if (noise % TREE_CHANCE == 0)
        {
            Graphics->DrawObject3D<dead_tree, deadtree01, false>(position);
        }
        else if (noise % GRASS_CHANCE == 0)
        {
            Graphics->DrawObject3D<grass, grass01, false>(position);
        }
    }
    
    inline void UpdateLight() const
    {
        static SVECTOR sunRotation{0 , 1624, 0};        
        const auto color = AmbientLightController::color;
        MATRIX color_mtx = {
            ((short)color.r<<1)<<4 /*163056*/, 0, 0,	/* Red   */
            ((short)color.g<<1)<<4 /*2912*/, 0, 0,	/* Green */
            ((short)color.b<<1)<<4 /*2032*/, 0, 0	/* Blue  */
        };
        MATRIX omtx{0};	// Object Matrix
        //191>>1,182>>1,127>>1
        sunRotation.vz++;
        if(sunRotation.vz > 4096)
            sunRotation.vz = 0;
        RotMatrix(&sunRotation, &omtx);
        gte_SetLightMatrix(&omtx);
        gte_SetColorMatrix(&color_mtx);
    }
    int Render(int mapPosX, int mapPosZ, MATRIX &mtx, const MATRIX& lightMTX)
    {
        Update(mapPosX, mapPosZ);        
        UpdateLight();
    
        PushMatrix();
        gte_SetTransMatrix(&mtx);

        for (int x = 0; x < MAP_SIZE - 1; ++x)
        {
            const auto x512 = x << 9;
            const short displacementX = (mapPosX >> 3) % 512;

            for (int z = 0; z < MAP_SIZE - 1; ++z)
            {
                const short x1z1 = map[x + 1][z + 1];
                const short x0z0 = map[x    ][z    ];
                const short x1z0 = map[x + 1][z    ];
                const short x0z1 = map[x    ][z + 1];

                const short z512 = z << 9;
                const short displacementZ = (mapPosZ >> 3) % 512; //mapPosZ%500;
                const short vx = x512 - displacementX;
                const short vz = z512 - displacementZ;

                const SVector3D vertices[4] = {
                    SVector3D(256 + vx, x1z0, -256 + vz),  //0
                    SVector3D(-256 + vx, x0z0, -256 + vz), //3
                    SVector3D(256 + vx, x1z1, 256 + vz),   //1
                    SVector3D(-256 + vx, x0z1, 256 + vz),  //2
                };

                const Vector3D side1 = {vertices[1].vx - vertices[0].vx, vertices[1].vy - vertices[0].vy, vertices[1].vz - vertices[0].vz};
                const Vector3D side2 = {vertices[2].vx - vertices[0].vx, vertices[2].vy - vertices[0].vy, vertices[2].vz - vertices[0].vz};
                /*const SVector3D normal = MathUtils::cross(
                    {side1.vx, side1.vy, side1.vz},
                    {side2.vx, side2.vy, side2.vz});*/
                
                const SVector3D normal = Vector3D::crossProduct(side1, side2).normalise();

                Graphics->Draw<POLY_FT4, false>(vertices, normal, &grass_tile01_texture);

                RenderDecorations(decorations[x][z], {vx, x1z0, vz});
            }
        }
        PopMatrix();

        const auto med = (map[CENTER_X][CENTER_Z] + map[CENTER_X+1][CENTER_Z] + map[CENTER_X][CENTER_Z+1] +map[CENTER_X+1][CENTER_Z+1])>>2;
        int targetY = med - 1024;

        return targetY;
    }

    ~Map()
    {
    }

private:
    void GenerateFullMap(int mapPosX, int mapPosZ)
    {
        const auto posx = mapPosX >> 12;
        const auto posz = mapPosZ >> 12;
        for (int z = 0; z < MAP_SIZE; ++z)
        {
            for (int x = 0; x < MAP_SIZE; ++x)
            {
                map[x][z] = GenerateHeight(x + posx, z + posz);
                decorations[x][z] = NoiseGenerator::Generate(x + posx, z + posz, 32) & 0xFF;
            }
        }
    }

    inline unsigned char GenerateDecorations(int x, int z)
    {
        constexpr int seed = 32;
        return NoiseGenerator::Generate(x, z, seed) & 0xFF;
    }

    inline int GenerateHeight(int x, int z)
    {
        constexpr PerlinNoise perlin;
        return (((perlin.noise2d(x << 12, z << 12) << 2) + (perlin.noise2d((x << 13) << 1, (z << 13) << 1) << 1) + perlin.noise2d((x << 13) << 2, (z << 13) << 2)));
    }

    inline void OffsetMapLeft()
    {
        for (int x = 1; x < MAP_SIZE; ++x)
        {
            for (int z = 0; z < MAP_SIZE; ++z)
            {
                map[x - 1][z] = map[x][z];
                decorations[x - 1][z] = decorations[x][z];
            }
        }
    }

    inline void OffsetMapRight()
    {
        for (int x = MAP_SIZE - 1; x > 0 - 1; --x)
        {
            for (int z = 0; z < MAP_SIZE; ++z)
            {
                map[x][z] = map[x - 1][z];
                decorations[x][z] = decorations[x - 1][z];
            }
        }
    }

    inline void OffsetUp()
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            for (int z = 1; z < MAP_SIZE; ++z)
            {
                map[x][z - 1] = map[x][z];
                decorations[x][z - 1] = decorations[x][z];
            }
        }
    }

    inline void OffsetDown()
    {
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            for (int z = MAP_SIZE - 1; z > 0; --z)
            {
                map[x][z] = map[x][z - 1];
                decorations[x][z] = decorations[x][z - 1];
            }
        }
    }

    inline void OffsetAndGenerateLeft(const int mapPosX, const int mapPosZ)
    {
        OffsetMapLeft();

        const int posx = mapPosX >> 12;
        const int posz = mapPosZ >> 12;
        const int x = MAP_SIZE - 1;
        for (int z = 0; z < MAP_SIZE; ++z)
        {
            map[x][z] = GenerateHeight(x + posx, z + posz);
            decorations[x][z] = GenerateDecorations(x + posx, z + posz);
        }
        LastPosX = posx;
    }

    inline void OffsetAndGenerateRight(const int mapPosX, const int mapPosZ)
    {
        OffsetMapRight();

        const int posx = mapPosX >> 12;
        const int posz = mapPosZ >> 12;
        const int x = 0;
        for (int z = 0; z < MAP_SIZE; ++z)
        {
            map[x][z] = GenerateHeight(x + posx, z + posz);
            decorations[x][z] = GenerateDecorations(x + posx, z + posz);
        }
        LastPosX = posx;
    }

    inline void OffsetAndGenerateUp(const int mapPosX, const int mapPosZ)
    {
        OffsetUp();

        const auto posx = mapPosX >> 12;
        const auto posz = mapPosZ >> 12;
        const int z = MAP_SIZE - 1;
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            map[x][z] = GenerateHeight(x + posx, z + posz);
            decorations[x][z] = GenerateDecorations(x + posx, z + posz);
        }
        LastPosZ = mapPosZ >> 12;
    }

    inline void OffsetAndGenerateDown(const int mapPosX, const int mapPosZ)
    {
        OffsetDown();

        const auto posx = mapPosX >> 12;
        const auto posz = mapPosZ >> 12;
        const int z = 0;
        for (int x = 0; x < MAP_SIZE; ++x)
        {
            map[x][z] = GenerateHeight(x + posx, z + posz);
            decorations[x][z] = GenerateDecorations(x + posx, z + posz);
        }
        LastPosZ = mapPosZ >> 12;
    }

    inline void Update(int mapPosX, int mapPosZ)
    {
        const auto mapXoffset = mapPosX >> 12;
        const auto mapZoffset = mapPosZ >> 12;
        if (mapXoffset > LastPosX)
        {
            OffsetAndGenerateLeft(mapPosX, mapPosZ);
        }
        else if (mapXoffset < LastPosX)
        {
            OffsetAndGenerateRight(mapPosX, mapPosZ);
        }

        if (mapZoffset > LastPosZ)
        {
            OffsetAndGenerateUp(mapPosX, mapPosZ);
        }
        else if (mapZoffset < LastPosZ)
        {
            OffsetAndGenerateDown(mapPosX, mapPosZ);
        }
    }
};

#endif // _MAP_H_