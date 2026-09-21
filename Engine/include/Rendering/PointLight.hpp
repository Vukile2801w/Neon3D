#ifndef NEON_POINTLIGHT
#define NEON_POINTLIGHT

#include "Rendering/Light.hpp"

namespace Neon
{
    class PointLight : public Light
    {
    public:
        using Light::Light;

        std::string getTypeName() const override { return "Point"; }

        void onSerialize(nlohmann::json &out) const override;

        glm::vec3 position{0.0f};

        // Distance at which the light's contribution is considered negligible.
        // Not fed into the shader's attenuation formula yet (Renderer still uses
        // the fixed falloff already in shader.frag) - kept here so it round-trips
        // through save/load once the Renderer/shader side is extended.
        float range{10.0f};
    };
}

#endif
