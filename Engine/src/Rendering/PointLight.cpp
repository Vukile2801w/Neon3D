#include "Rendering/PointLight.hpp"

#include "json.hpp"

namespace Neon
{
    namespace
    {
        nlohmann::json vec3ToJson(const glm::vec3 &v)
        {
            return nlohmann::json::array({v.x, v.y, v.z});
        }
    }

    void PointLight::onSerialize(nlohmann::json &out) const
    {
        out["color"] = vec3ToJson(color);
        out["intensity"] = intensity;

        out["position"] = vec3ToJson(position);
        out["range"] = range;
    }
}
