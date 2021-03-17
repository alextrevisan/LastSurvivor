#include "obj2c.h"
#include <cmath>

obj2c::obj2c(const fs::path& path, const fs::path& output, const int verticeScale, const int textureSizeU, const int textureSizeV, const int normalScale)
:OutputFile(output), VerticeScale(verticeScale), TextureSizeU(textureSizeU-1), TextureSizeV(textureSizeV-1), NormalScale(normalScale-1)
{
    processLines(path);
}

void obj2c::generate()
{
    std::ofstream file(OutputFile);

    generateFileStart(file);

    generateVertices(file);
    generateUVs(file);
    generateNormals(file);

    generateQuads(file);
    generateTris(file);

    generateEndFile(file);

}

void obj2c::generateFileStart(std::ofstream& file)
{
    file << "#ifndef __"<<OutputFile.stem().string()<<"_h_\n";
    file << "#define __"<<OutputFile.stem().string()<<"_h_\n\n";
    file << "#include <psxgte.h>\n\n";
    file << "struct "<<OutputFile.stem().string()<<" {\n\n";
}

void obj2c::generateEndFile(std::ofstream& file)
{
    file<<"\tint x = 0, y = 0, z = 0;\n";
    file<<"\tTIM_IMAGE* texture = nullptr;\n";
    file<<"};\n\n";
    file<<"#endif //__"<<OutputFile.stem().string()<<"_h_\n";

}

void obj2c::generateVertices(std::ofstream& file)
{
    file << "\tconst SVECTOR vertices["<<obj.vertices.size()<<"] = {\n";
    for(const auto& vertice: obj.vertices)
    {
        file<<"\t\t{ "<<vertice[0]<<", "<<vertice[1]<<", "<<vertice[2]<<" },\n";
    }
    file<<"\t};\n\n";
}

void obj2c::generateUVs(std::ofstream& file)
{
    file << "\tconst DVECTOR uvs["<<obj.uvs.size()<<"] = {\n";
    for(const auto& uv: obj.uvs)
    {
        file<<"\t\t{ "<<uv[0]<<", "<<uv[1]<<" },\n";
    }
    file<<"\t};\n\n";
}

void obj2c::generateNormals(std::ofstream& file)
{
    file << "\tconst SVECTOR normals["<<obj.normals.size()<<"] = {\n";
    for(const auto& normal: obj.normals)
    {
        file<<"\t\t{ "<<normal[0]<<", "<<normal[1]<<", "<<normal[2]<<" },\n";
    }
    file<<"\t};\n\n";
}

void obj2c::generateQuads(std::ofstream& file)
{
    file<<"\tstruct face4\n";
    file<<"\t{\n";
    file<<"\t\t"<< (true ? "short" : "char") <<" vertice0,uv0,normal0;\n";
    file<<"\t\t"<< (true ? "short" : "char") <<" vertice1,uv1,normal1;\n";
    file<<"\t\t"<< (true ? "short" : "char") <<" vertice2,uv2,normal2;\n";
    file<<"\t\t"<< (true ? "short" : "char") <<" vertice3,uv3,normal3;\n";
    file<<"\t};\n\n";
    file << "\tconst face4 quads["<<obj.quads.size()<<"] = {\n";
    for(const auto& quad: obj.quads)
    {
        file<<"\t\t{ "<<quad.vertex0<<", "<<quad.uv0<<", "<<quad.normal0<<", "
        <<quad.vertex1<<", "<<quad.uv1<<", "<<quad.normal1<<", "
        <<quad.vertex2<<", "<<quad.uv2<<", "<<quad.normal2<<", "
        <<quad.vertex3<<", "<<quad.uv3<<", "<<quad.normal3
        <<" },\n";
    }
    file<<"\t};\n\n";
}

void obj2c::generateTris(std::ofstream& file)
{
    file<<"\tstruct face3\n";
    file<<"\t{\n";
    file<<"\t\tunsigned "<< (true ? "short" : "char") <<" vertice0,uv0,normal0;\n";
    file<<"\t\tunsigned "<< (true ? "short" : "char") <<" vertice1,uv1,normal1;\n";
    file<<"\t\tunsigned "<< (true ? "short" : "char") <<" vertice2,uv2,normal2;\n";
    file<<"\t};\n\n";
    file << "\tconst face3 tris["<<obj.tris.size()<<"] = {\n";
    for(const auto& tri: obj.tris)
    {
        file<<"\t\t{ "<<tri.vertex0<<", "<<tri.uv0<<", "<<tri.normal0<<", "
        <<tri.vertex1<<", "<<tri.uv1<<", "<<tri.normal1<<", "
        <<tri.vertex2<<", "<<tri.uv2<<", "<<tri.normal2
        <<" },\n";
    }
    file<<"\t};\n\n";
}


void obj2c::processLines(const fs::path& path)
{
    std::ifstream file(path, std::ios::in);
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            // using printf() in all tests for consistency
            //std::cout<<line<<'\n';
            loadMTL(line);
            loadVertices(line);
            loadUVs(line);
            loadNormals(line);
            loadQuads(line);
            loadTris(line);
        }
        file.close();
    }
}

void obj2c::loadMTL(const std::string& line)
{
    if(line.rfind("mtllib", 0) == 0)
        MTLFileName = split(line, " ")[1];
}

void obj2c::loadVertices(const std::string& line)
{
    if(line.rfind("v ", 0) != 0)
        return;

    const std::vector<std::string> items = split(line, " ");
    if(items.size() < 4)
        return;

    const int x = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[1]) * VerticeScale));

    const int y = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[2]) * VerticeScale));

    const int z = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[3]) * VerticeScale));

    obj.vertices.push_back({x,y,z});
}

void obj2c::loadUVs(const std::string& line)
{
    if(line.rfind("vt ", 0) != 0)
        return;

    const std::vector<std::string> items = split(line, " ");
    if(items.size() < 3)
        return;

    //UVs are inverted in blender
    const int u = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[1]) * TextureSizeU));

    const int v = TextureSizeV - static_cast<int>(
                        std::nearbyint(
                            std::stof(items[2]) * TextureSizeV));


    obj.uvs.push_back({u,v});
}

void obj2c::loadNormals(const std::string& line)
{
    if(line.rfind("vn ", 0) != 0)
        return;

    const std::vector<std::string> items = split(line, " ");
    if(items.size() < 4)
        return;

    const int x = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[1]) * NormalScale));

    const int y = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[2]) * NormalScale));

    const int z = static_cast<int>(
                        std::nearbyint(
                            std::stof(items[3]) * NormalScale));

    obj.normals.push_back({x,y,z});
}

void obj2c::loadQuads(const std::string& line)
{
    if(line.rfind("f ", 0) != 0)
        return;

    const std::vector<std::string> items = split(line, " ");

    if(items.size() != 5)
        return;


    const std::vector<std::string> part1 = split(items[1], "/");
    const std::vector<std::string> part2 = split(items[2], "/");
    const std::vector<std::string> part3 = split(items[3], "/");
    const std::vector<std::string> part4 = split(items[4], "/");

    if(part1.size() < 3 || part2.size() < 3 || part3.size() < 3 || part4.size() < 3)
    {
        return;
    }

    const int vertex1 = std::stoi(part1[0]) - 1;
    const int uv1 = std::stoi(part1[1]) - 1;
    const int normal1 = std::stoi(part1[2]) - 1;

    const int vertex2 = std::stoi(part2[0]) - 1;
    const int uv2 = std::stoi(part2[1]) - 1;
    const int normal2 = std::stoi(part2[2]) - 1;

    const int vertex3 = std::stoi(part3[0]) - 1;
    const int uv3 = std::stoi(part3[1]) - 1;
    const int normal3 = std::stoi(part3[2]) - 1;

    const int vertex4 = std::stoi(part4[0]) - 1;
    const int uv4 = std::stoi(part4[1]) - 1;
    const int normal4 = std::stoi(part4[2]) - 1;

    obj.quads.push_back({
        vertex1,uv1,normal1,
        vertex2,uv2,normal2,
        vertex3,uv3,normal3,
        vertex4,uv4,normal4
        });


}

void obj2c::loadTris(const std::string& line)
{
    if(line.rfind("f ", 0) != 0)
        return;

    const std::vector<std::string> items = split(line, " ");

    if(items.size() != 4)
        return;


    const std::vector<std::string> part1 = split(items[1], "/");
    const std::vector<std::string> part2 = split(items[2], "/");
    const std::vector<std::string> part3 = split(items[3], "/");

    if(part1.size() < 3 || part2.size() < 3 || part3.size() < 3)
    {
        return;
    }

    const int vertex1 = std::stoi(part1[0]) - 1;
    const int uv1 = std::stoi(part1[1]) - 1;
    const int normal1 = std::stoi(part1[2]) - 1;

    const int vertex2 = std::stoi(part2[0]) - 1;
    const int uv2 = std::stoi(part2[1]) - 1;
    const int normal2 = std::stoi(part2[2]) - 1;

    const int vertex3 = std::stoi(part3[0]) - 1;
    const int uv3 = std::stoi(part3[1]) - 1;
    const int normal3 = std::stoi(part3[2]) - 1;

    obj.tris.push_back({
        vertex1,uv1,normal1,
        vertex2,uv2,normal2,
        vertex3,uv3,normal3
        });
}

std::vector<std::string> obj2c::split(const std::string& data, const std::string& delimiter)
{
    std::string s = data;
    std::vector<std::string> returnData;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        token = s.substr(0, pos);
        //std::cout << token << std::endl;
        returnData.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    token = s.substr(0, s.size());
    returnData.push_back(token);
    return returnData;
}

