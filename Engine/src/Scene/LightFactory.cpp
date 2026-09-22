#include "Scene/LightFactory.hpp"

#include "Assert.hpp"
#include "Debug/Logging.hpp"

#include "Rendering/DirectionalLight.hpp"
#include "Rendering/PointLight.hpp"
#include "Rendering/SpotLight.hpp"

#include "Scene/Scene.hpp"

#include "json.hpp"

namespace Neon
{
    namespace
    {
        glm::vec3 jsonToVec3(const nlohmann::json &node, const glm::vec3 &fallback)
        {
            if (!node.is_array() || node.size() != 3)
                return fallback;

            return glm::vec3(
                node[0].get<float>(),
                node[1].get<float>(),
                node[2].get<float>());
        }

        // Built-in light types are Engine-side (unlike Sandbox-side GameObject
        // subclasses like Cube), so Engine registers them itself rather than relying
        // on a consumer's onStart(). Runs once, before main(), the same
        // "self-registering helper" pattern GameObjectFactory.hpp's doc comment
        // already allows for.
        bool registerBuiltinLightTypes()
        {
            LightFactory::registerType(
                "Point",
                [](Scene &scene, const nlohmann::json &node) -> Light *
                {
                    PointLight *light = scene.createLight<PointLight>();

                    light->color = jsonToVec3(node.value("color", nlohmann::json()), light->color);
                    light->intensity = node.value("intensity", light->intensity);
                    light->position = jsonToVec3(node.value("position", nlohmann::json()), light->position);
                    light->range = node.value("range", light->range);

                    return light;
                });

            LightFactory::registerType(
                "Spot",
                [](Scene &scene, const nlohmann::json &node) -> Light *
                {
                    SpotLight *light = scene.createLight<SpotLight>();

                    light->color = jsonToVec3(node.value("color", nlohmann::json()), light->color);
                    light->intensity = node.value("intensity", light->intensity);
                    light->position = jsonToVec3(node.value("position", nlohmann::json()), light->position);
                    light->direction = jsonToVec3(node.value("direction", nlohmann::json()), light->direction);
                    light->range = node.value("range", light->range);
                    light->innerCone = node.value("innerCone", light->innerCone);
                    light->outerCone = node.value("outerCone", light->outerCone);

                    return light;
                });

            LightFactory::registerType(
                "Directional",
                [](Scene &scene, const nlohmann::json &node) -> Light *
                {
                    DirectionalLight *light = scene.createLight<DirectionalLight>();

                    light->color = jsonToVec3(node.value("color", nlohmann::json()), light->color);
                    light->intensity = node.value("intensity", light->intensity);
                    light->direction = jsonToVec3(node.value("direction", nlohmann::json()), light->direction);

                    return light;
                });

            return true;
        }

        const bool builtinLightTypesRegistered = registerBuiltinLightTypes();
    }

    std::unordered_map<std::string, LightFactory::CreateFn> &LightFactory::registry()
    {
        static std::unordered_map<std::string, CreateFn> instance;
        return instance;
    }

    void LightFactory::registerType(const std::string &typeName, CreateFn fn)
    {
        NEON_ASSERT(fn != nullptr, "LightFactory::registerType called with a null constructor function");

        if (registry().count(typeName))
            Logging::Warning("LightFactory: type '" + typeName + "' registered more than once, overwriting");

        registry()[typeName] = std::move(fn);

        Logging::Info("LightFactory: registered type '" + typeName + "'");
    }

    Light *LightFactory::create(
        const std::string &typeName,
        Scene &scene,
        const nlohmann::json &node)
    {
        auto it = registry().find(typeName);
        if (it == registry().end())
            return nullptr;

        return it->second(scene, node);
    }

    bool LightFactory::isRegistered(const std::string &typeName)
    {
        return registry().count(typeName) != 0;
    }
}
