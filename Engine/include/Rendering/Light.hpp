#ifndef NEON_LIGHT
#define NEON_LIGHT

#include "glm.hpp"

namespace Neon
{
    class Light
    {
    public:
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    };
}

#endif
