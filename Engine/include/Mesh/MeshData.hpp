#ifndef NEON_MESH_DATA
#define NEON_MESH_DATA

#include <vector>
#include "glm.hpp"

namespace Neon
{
    struct MeshVertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
        glm::vec3 tangent;
    };

    struct MeshData
    {
        std::vector<MeshVertex> vertices;
        std::vector<unsigned int> indices;
    };
}

#endif