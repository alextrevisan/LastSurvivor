#ifndef _OBJ2C_H_
#define _OBJ2C_H_

#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>

namespace fs = std::filesystem;

class obj2c
{
    public:
        obj2c(const fs::path& path, const fs::path& output, const int verticeScale, const int textureSizeU, const int textureSizeV, const int normalScale);
        void generate();

    private:
        void processLines(const fs::path& path);

        void generateFileStart(std::ofstream& file);
        void generateEndFile(std::ofstream& file);

        void generateVertices(std::ofstream& file);
        void generateUVs(std::ofstream& file);
        void generateNormals(std::ofstream& file);
        void generateQuads(std::ofstream& file);
        void generateTris(std::ofstream& file);


        void loadMTL(const std::string& line);
        void loadVertices(const std::string& line);
        void loadUVs(const std::string& line);
        void loadNormals(const std::string& line);
        void loadQuads(const std::string& line);
        void loadTris(const std::string& line);

        std::string MTLFileName;
        const fs::path OutputFile;
        const int VerticeScale;
        const int TextureSizeU;
        const int TextureSizeV;
        const int NormalScale;


        struct face
        {
            int vertex0,uv0,normal0;
            int vertex1,uv1,normal1;
            int vertex2,uv2,normal2;
            int vertex3,uv3,normal3;
        };

        struct obj
        {
            std::vector<std::array<int,3>> vertices;
            std::vector<std::array<int,2>> uvs;
            std::vector<std::array<int,3>> normals;
            std::vector<face> quads;
            std::vector<face> tris;
        }obj;

        std::vector<std::string> split(const std::string& data, const std::string& delimiter);
};

#endif // _OBJ2C_H_
