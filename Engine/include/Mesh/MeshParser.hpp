#ifndef NEON_MESH_PARSER
#define NEON_MESH_PARSER

#include <filesystem>

#include "Mesh/MeshData.hpp"

namespace Neon
{
    class MeshParser
    {
    public:
        static MeshData parse(
            const std::filesystem::path &path);
    };
}

#endif