#ifndef NEON_TRANSFORM
#define NEON_TRANSFORM

#include "glm.hpp"

namespace Neon
{
    class Transform
    {
    public:
        Transform(glm::vec3 position = glm::vec3(0.0f), glm::vec3 rotation = glm::vec3(0.0f), glm::vec3 scale = glm::vec3(1.0f));

        glm::vec3 position{0.0f};
        glm::vec3 rotation{0.0f};
        glm::vec3 scale{1.0f};

        glm::mat4 getMatrix() const;
    };
}

#endif