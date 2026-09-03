#include "Scene/SceneSerializer.hpp"

#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

#include "Assert.hpp"
#include "Logging.hpp"

#include "Assets/AssetManager.hpp"
#include "Mesh/Mesh.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/GameObjectFactory.hpp"
#include "Scene/Scene.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/Renderer.hpp"

#include "json.hpp"

namespace Neon
{
    using json = nlohmann::json;

    namespace
    {
        json vec3ToJson(const glm::vec3 &v)
        {
            return json::array({v.x, v.y, v.z});
        }

        glm::vec3 jsonToVec3(const json &node, const glm::vec3 &fallback)
        {
            if (!node.is_array() || node.size() != 3)
                return fallback;

            return glm::vec3(
                node[0].get<float>(),
                node[1].get<float>(),
                node[2].get<float>());
        }

        json materialPropertyToJson(const MaterialProperty &property)
        {
            return std::visit(
                [](const auto &value) -> json
                {
                    using T = std::decay_t<decltype(value)>;

                    if constexpr (std::is_same_v<T, glm::vec2>)
                    {
                        return json::array({value.x,
                                            value.y});
                    }
                    else if constexpr (std::is_same_v<T, glm::vec3>)
                    {
                        return json::array({value.x,
                                            value.y,
                                            value.z});
                    }
                    else if constexpr (std::is_same_v<T, glm::vec4>)
                    {
                        return json::array({value.x,
                                            value.y,
                                            value.z,
                                            value.w});
                    }
                    else if constexpr (std::is_same_v<T, glm::mat3>)
                    {
                        json result = json::array();

                        for (int column = 0; column < 3; ++column)
                        {
                            result.push_back({value[column][0],
                                              value[column][1],
                                              value[column][2]});
                        }

                        return result;
                    }
                    else if constexpr (std::is_same_v<T, glm::mat4>)
                    {
                        json result = json::array();

                        for (int column = 0; column < 4; ++column)
                        {
                            result.push_back({value[column][0],
                                              value[column][1],
                                              value[column][2],
                                              value[column][3]});
                        }

                        return result;
                    }
                    else if constexpr (std::is_same_v<T, MaterialLight>)
                    {
                        if (value.light == nullptr)
                            return json{
                                {"index", value.index}};

                        return json{
                            {"index", value.index},
                            {"light", {{"position", {value.light->position.x, value.light->position.y, value.light->position.z}}, {"color", {value.light->color.x, value.light->color.y, value.light->color.z}}, {"intensity", value.light->intensity}}}};
                    }
                    else
                    {
                        return value;
                    }
                },
                property);
        }
        MaterialProperty jsonToMaterialProperty(const json &value)
        {
            if (value.is_boolean())
                return value.get<bool>();

            if (value.is_number_unsigned())
                return value.get<unsigned int>();

            if (value.is_number_integer())
                return value.get<int>();

            if (value.is_number_float())
                return value.get<float>();

            if (value.is_object() &&
                value.contains("index") &&
                value.contains("light"))
            {
                const auto &lightNode = value["light"];

                if (!lightNode.is_object())
                    return 0.0f;

                auto light = std::make_unique<Light>();

                light->position = jsonToVec3(
                    lightNode.value("position", json()),
                    glm::vec3(0.0f));

                light->color = jsonToVec3(
                    lightNode.value("color", json()),
                    glm::vec3(1.0f));

                light->intensity =
                    lightNode.value("intensity", 1.0f);

                // OVO još ne možeš vratiti kao MaterialLight.light
                // jer Renderer još nije napravio Light.
            }

            if (value.is_array())
            {
                // vec2
                if (value.size() == 2 &&
                    !value[0].is_array())
                {
                    return glm::vec2(
                        value[0].get<float>(),
                        value[1].get<float>());
                }

                // vec3
                if (value.size() == 3 &&
                    !value[0].is_array())
                {
                    return glm::vec3(
                        value[0].get<float>(),
                        value[1].get<float>(),
                        value[2].get<float>());
                }

                // vec4
                if (value.size() == 4 &&
                    !value[0].is_array())
                {
                    return glm::vec4(
                        value[0].get<float>(),
                        value[1].get<float>(),
                        value[2].get<float>(),
                        value[3].get<float>());
                }

                // mat3
                if (value.size() == 3 &&
                    value[0].is_array())
                {
                    glm::mat3 result(1.0f);

                    for (int column = 0; column < 3; ++column)
                    {
                        for (int row = 0; row < 3; ++row)
                        {
                            result[column][row] =
                                value[column][row].get<float>();
                        }
                    }

                    return result;
                }

                // mat4
                if (value.size() == 4 &&
                    value[0].is_array())
                {
                    glm::mat4 result(1.0f);

                    for (int column = 0; column < 4; ++column)
                    {
                        for (int row = 0; row < 4; ++row)
                        {
                            result[column][row] =
                                value[column][row].get<float>();
                        }
                    }

                    return result;
                }
            }

            Logging::Warning(
                "SceneSerializer::load - unsupported material property value");

            return 0.0f;
        }

    }

    bool SceneSerializer::save(const Scene &scene, const std::string &path)
    {
        json root;
        root["version"] = 1;
        root["objects"] = json::array();

        // Maps each already-written object to its index in the output array, so a
        // child's "parent" field can reference it by index rather than by name (no
        // uniqueness requirement on GameObject::name this way). Scene::createGameObject
        // requires a parent to already exist before a child can be constructed with it,
        // so getGameObjects() is guaranteed to list every parent before its children -
        // a single forward pass is enough.
        std::unordered_map<const GameObject *, size_t> indexOf;

        for (const auto &obj : scene.getGameObjects())
        {
            json node;
            node["type"] = obj->getTypeName();
            node["name"] = obj->name;

            node["transform"]["position"] = vec3ToJson(obj->transform.position);
            node["transform"]["rotation"] = vec3ToJson(obj->transform.rotation);
            node["transform"]["scale"] = vec3ToJson(obj->transform.scale);

            if (!obj->meshPath.empty())
                node["mesh"] = obj->meshPath;

            // Material
            if (obj->material != nullptr)
            {
                const Material &material = *obj->material;

                json materialNode;

                // Shader
                if (material.getShader())
                {
                    const auto &shader = material.getShader();
                    std::string path = shader->getStageSourcePath(ShaderStage::Type::Vertex);

                    if (path != "none")
                        materialNode["shader"]["vertex"] = path;

                    path = shader->getStageSourcePath(ShaderStage::Type::Fragment);

                    if (path != "none")
                        materialNode["shader"]["fragment"] = path;
                }

                // Properties
                for (const auto &[name, value] : material.getProperties())
                {
                    materialNode["properties"][name] =
                        materialPropertyToJson(value);
                }

                // Textures
                for (const auto &slot : material.getTextures())
                {
                    if (slot.texture)
                        materialNode["textures"][slot.name] =
                            slot.texture->getPath();
                }

                node["material"] = std::move(materialNode);
            }

            if (obj->getParent() != nullptr)
            {
                auto parentIt = indexOf.find(obj->getParent());

                NEON_ASSERT(
                    parentIt != indexOf.end(),
                    "SceneSerializer::save - parent was not written before its child; "
                    "Scene should guarantee parents are constructed before their children");

                node["parent"] = parentIt->second;
            }

            // Subclass-specific extra fields (error.g. Cube's isLightSource/color) go
            // directly into 'node' alongside the base fields above - see
            // GameObject::onSerialize()'s doc comment for the reserved-key caveat.
            obj->onSerialize(node);

            indexOf[obj.get()] = root["objects"].size();
            root["objects"].push_back(std::move(node));
        }

        std::ofstream file(path);
        if (!file.is_open())
        {
            Logging::Error("SceneSerializer::save - failed to open file for writing: " + path);
            return false;
        }

        file << root.dump(4);

        Logging::Info("SceneSerializer::save - wrote scene to " + path);
        return true;
    }

    bool SceneSerializer::load(Scene &outScene, const std::string &path, AssetManager &assetManager, Renderer &renderer)
    {
        std::ifstream file(path);
        if (!file.is_open())
        {
            Logging::Error("SceneSerializer::load - failed to open file for reading: " + path);
            return false;
        }

        json root;
        try
        {
            file >> root;
        }
        catch (const json::parse_error &error)
        {
            Logging::Error("SceneSerializer::load - malformed JSON in " + path + ": " + std::string(error.what()));
            return false;
        }

        if (!root.contains("objects") || !root["objects"].is_array())
        {
            Logging::Error("SceneSerializer::load - '" + path + "' has no 'objects' array");
            return false;
        }

        // Every object is constructed top-level first (parent = nullptr); parent/child
        // links are resolved in a second pass below via the public
        // GameObject::setParent(), so a GameObjectFactory::CreateFn never needs to
        // reason about hierarchy, and object order within the file doesn't matter.
        std::vector<GameObject *> created;
        created.reserve(root["objects"].size());

        for (const json &node : root["objects"])
        {
            std::string typeName = node.value("type", std::string("GameObject"));

            GameObject *obj = GameObjectFactory::create(typeName, outScene, node, assetManager);

            if (obj == nullptr)
            {
                if (typeName != "GameObject")
                    Logging::Warning(
                        "SceneSerializer::load - unrecognized type '" + typeName +
                        "', falling back to plain GameObject");

                obj = outScene.createGameObject<GameObject>(nullptr);
            }

            obj->name = node.value("name", std::string());

            if (node.contains("transform"))
            {
                const json &t = node["transform"];
                obj->transform.position = jsonToVec3(t.value("position", json()), obj->transform.position);
                obj->transform.rotation = jsonToVec3(t.value("rotation", json()), obj->transform.rotation);
                obj->transform.scale = jsonToVec3(t.value("scale", json()), obj->transform.scale);
            }

            if (node.contains("mesh"))
            {
                obj->meshPath = node["mesh"].get<std::string>();
                obj->mesh = assetManager.load<Mesh>(obj->meshPath);

                if (!obj->mesh)
                    Logging::Warning(
                        "SceneSerializer::load - could not load mesh '" + obj->meshPath +
                        "' for object '" + obj->name + "'");
            }

            if (node.contains("material"))
            {
                const json &materialNode = node["material"];

                Material *material = obj->material;

                if (material != nullptr)
                {
                    if (materialNode.contains("properties"))
                    {
                        for (const auto &[name, value] :
                             materialNode["properties"].items())
                        {
                            material->setProperty(
                                name,
                                jsonToMaterialProperty(value));
                        }
                    }

                    if (materialNode.contains("textures"))
                    {
                        for (const auto &[name, value] :
                             materialNode["textures"].items())
                        {
                            std::string texturePath = value.get<std::string>();

                            auto texture =
                                assetManager.load<Texture>(texturePath);

                            if (texture)
                                material->setTexture(name, texture);
                        }
                    }

                    auto property = material->getProperty("u_IsLightSource");

                    if (std::holds_alternative<bool>(property) &&
                        std::get<bool>(property))
                    {
                        Logging::Info("Light!");
                        Light light;
                        light.position = obj->transform.position;
                        light.color = glm::vec3(1.0f);
                        light.intensity = 1.0f;

                        renderer.addLight(light);
                    }
                }
            }

            created.push_back(obj);
        }

        for (size_t i = 0; i < created.size(); ++i)
        {
            const json &node = root["objects"][i];
            if (!node.contains("parent"))
                continue;

            size_t parentIndex = node["parent"].get<size_t>();
            if (parentIndex >= created.size())
            {
                Logging::Warning(
                    "SceneSerializer::load - parent index " + std::to_string(parentIndex) +
                    " out of range for object '" + created[i]->name + "'");
                continue;
            }

            created[i]->setParent(created[parentIndex]);
        }

        Logging::Info("SceneSerializer::load - loaded scene from " + path);
        return true;
    }
}