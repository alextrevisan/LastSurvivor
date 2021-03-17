#include <iostream>
#include <filesystem>
#include <fstream>
#include "Args.h"
#include "obj2c.h"

namespace fs = std::filesystem;

void print_usage()
{
    std::cout<<"Usage: "<<'\n';
    std::cout<<"obj2c -in input.obj [params] [-o output.h]"<<'\n';
    std::cout<<"params:"<<'\n';
    std::cout<<"-vs <value>     - Specifies scale for vertex position. Defalt 100"<<'\n';
    std::cout<<"-tu <value>     - Specifies scale for (u) texture size (for UV coords). Default 64"<<'\n';
    std::cout<<"-tv <value>     - Specifies scale for (v) texture size (for UV coords). Default 64"<<'\n';
    std::cout<<"-ns <value>     - Specifies scale for normals. Defalt 4096"<<'\n';
    std::cout<<"PS.: Export the obj file with -Y up and +Z forward"<<'\n';
}

int main(int argc, char** argv)
{
    if(argc <= 1)
    {
        print_usage();
        return 0;
    }

    Args args(argc, argv);

    if(!args.Contains("-in"))
    {
        print_usage();
        return 0;
    }

    const fs::path path = args.Get<std::string>("-in");
    if(!fs::exists(path))
    {
        std::cout<<".obj file "<<path<<" not found"<<'\n';
        return 0;
    }

    const int vs = args.Get("-vs", 100);
    int ns = args.Get("-ns", 4096);
    int tu = args.Get("-tu", 64);
    int tv = args.Get("-tv", 64);
    const std::string output = args.Get("-o", path.stem().generic_string() + ".h");
    obj2c converter(path, output, vs, tu, tv, ns);
    converter.generate();

    return 0;
}
