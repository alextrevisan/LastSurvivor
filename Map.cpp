#include <Map.h>
#include <PerlinNoise.h>
#include <MathUtils.h>

Map::MapMesh& Map::GenerateMesh(const int mapIdxX, const int mapIdxZ)
{
    unsigned int meshIdx = 0;
    const int camZ = mapIdxX/SCALE;
    const int camX = mapIdxZ/SCALE;
    if(lastMapX != camX || lastMapY != camZ)
    {
        PerlinNoise perlin;
        lastMapX = camX;
        lastMapY = camZ;
        int currentMapX = 0;
        constexpr auto mapYScale = 15;
        
        auto& mesh = mapMesh.mesh;
        auto& normals = mapMesh.normals;
        const auto MESH_SIZE = mapMesh.MESH_SIZE;

        for (int x = camX; x < camX+MESH_SIZE; x++)
        {
            int currentMapZ = 0;
            for (int y = camZ; y < camZ+MESH_SIZE; y++)
            {
                const int x0 = x<<13;
                const int y0 = (y)<<13;
                const int x1 = (x+1)<<13;
                const int y1 = (y+1)<<13;

                const auto x1y1 = perlin.noise2d(x1, y1)*mapYScale;
                const auto x0y0 = perlin.noise2d(x0, y0)*mapYScale;
                mesh[meshIdx] = {(currentMapX+1)*SCALE, x1y1, (currentMapZ+1)*SCALE, 0};
                mesh[meshIdx+1] = {currentMapX*SCALE, x0y0, currentMapZ*SCALE, 0};
                mesh[meshIdx+2] = {currentMapX*SCALE, perlin.noise2d(x0, y1)*mapYScale, (currentMapZ+1)*SCALE, 0};

                const SVECTOR side1 = {mesh[meshIdx+1].vx - mesh[meshIdx].vx, mesh[meshIdx+1].vy - mesh[meshIdx].vy, mesh[meshIdx+1].vz - mesh[meshIdx].vz };
                const SVECTOR side2 = {mesh[meshIdx+1].vx - mesh[meshIdx+2].vx, mesh[meshIdx+1].vy - mesh[meshIdx+2].vy, mesh[meshIdx+1].vz - mesh[meshIdx+2].vz };

                mesh[meshIdx+3] = {currentMapX*SCALE, x0y0, currentMapZ*SCALE, 0};
                mesh[meshIdx+4] = {(currentMapX+1)*SCALE, x1y1, (currentMapZ+1)*SCALE, 0};
                mesh[meshIdx+5] = {(currentMapX+1)*SCALE, perlin.noise2d(x1, y0)*mapYScale, (currentMapZ)*SCALE, 0};

                const SVECTOR side3 = {mesh[meshIdx+4].vx - mesh[meshIdx+3].vx, mesh[meshIdx+4].vy - mesh[meshIdx+3].vy, mesh[meshIdx+4].vz - mesh[meshIdx+3].vz };
                const SVECTOR side4 = {mesh[meshIdx+4].vx - mesh[meshIdx+5].vx, mesh[meshIdx+4].vy - mesh[meshIdx+5].vy, mesh[meshIdx+4].vz - mesh[meshIdx+5].vz };

                const int meshBy3 = meshIdx/3;
                normals[meshBy3] = MathUtils::cross(side1, side2);
                normals[meshBy3+1] = MathUtils::cross(side3, side4);
                
                meshIdx+=6;
                currentMapZ++;
            }
            currentMapX++;
        }
    }
    return mapMesh;
}

Map::Map(/* args */)
{
}

Map::~Map()
{
}
