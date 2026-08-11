#ifndef NEON_CAMERA
#define NEON_CAMERA

#include "glm.hpp"

namespace Neon
{
    class Camera
    {
    public:
        glm::vec3 position{0.0f, 0.0f, 3.0f};
        glm::vec3 rotation{0.0f};
        float FOV{45.0f};

        glm::mat4 getViewMatrix() const;
        glm::mat4 getProjectionMatrix(float aspectRatio) const;
    };
}

#endif