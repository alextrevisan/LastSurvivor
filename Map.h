#ifndef _MAP_H_
#define _MAP_H_
#include <psxgte.h>

class Map
{
public:
    typedef struct MapMesh
    {
        static constexpr unsigned MESH_SIZE = 24;
        SVECTOR mesh[MESH_SIZE*MESH_SIZE*6];
        SVECTOR normals[MESH_SIZE*MESH_SIZE*2];
        const CVECTOR colors[1] = {{128, 128, 128, 0}};
        const int sizes[1] = {MESH_SIZE*MESH_SIZE*6};
        const int count = 1;
    }MapMesh;

    MapMesh& GenerateMesh(const int mapIdxX, const int mapIdxZ);

    static constexpr unsigned SCALE = 500;

    Map(/* args */);
    ~Map();
private:
    int lastMapX = -515, lastMapY = -515;
    MapMesh mapMesh;
};

#endif //_MAP_H_