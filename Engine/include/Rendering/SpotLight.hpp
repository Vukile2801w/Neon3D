#ifndef NEON_SPOTLIGHT
#define NEON_SPOTLIGHT

#include "Rendering/Light.hpp"

namespace Neon
{
    class SpotLight : public Light
    {
    public:
        using Light::Light;

        std::string getTypeName() const override { return "Spot"; }

        void onSerialize(nlohmann::json &out) const override;

        glm::vec3 position{0.0f};
        glm::vec3 direction{0.0f, -1.0f, 0.0f};

        // Same purpose as PointLight::range - distance at which the light's
        // contribution falls off to zero, independent of the cone angle falloff.
        float range{10.0f};

        float innerCone{12.5f};
        float outerCone{17.5f};
    };
}

#endif