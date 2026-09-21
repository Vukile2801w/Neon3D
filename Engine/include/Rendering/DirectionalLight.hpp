#ifndef NEON_DIRECTIONALLIGHT
#define NEON_DIRECTIONALLIGHT

#include "Rendering/Light.hpp"

namespace Neon
{
    class DirectionalLight : public Light
    {
    public:
        using Light::Light;

        std::string getTypeName() const override { return "Directional"; }

        void onSerialize(nlohmann::json &out) const override;

        glm::vec3 direction{0.0f, -1.0f, 0.0f};
    };
}

#endif
