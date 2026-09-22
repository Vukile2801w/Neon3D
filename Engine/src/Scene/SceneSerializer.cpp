#include "Scene/SceneSerializer.hpp"

#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

#include "Assert.hpp"
#include "Debug/Logging.hpp"

#include "Assets/AssetManager.hpp"
#include "Mesh/Mesh.hpp"
#include "Rendering/Light.hpp"
#include "Scene/GameObject.hpp"
#include "Scene/GameObjectFactory.hpp"
#include "Scene/LightFactory.hpp"
#include "Scene/Scene.hpp"
#include "Rendering/Shader.hpp"

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

        // Splits a "<Category>/<TypeName>" entity type string into its two halves.
        // This is the one place SceneSerializer knows the "GameObject/"/"Light/"
        // prefix convention exists at all - GameObjectFactory/LightFactory registry
        // keys never include it (see their own header comments).
        struct EntityType
        {
            std::string category;
            std::string typeName;
        };

        EntityType splitEntityType(const std::string &type)
        {
            size_t slash = type.find('/');

            if (slash == std::string::npos)
                return EntityType{std::string(), type};

            return EntityType{type.substr(0, slash), type.substr(slash + 1)};
        }

    }

    bool SceneSerializer::save(const Scene &scene, const std::string &path)
    {
        json root;
        root["version"] = 2;
        root["entities"] = json::array();

        // Maps each already-written GameObject to its index in the output array, so
        // a child's "parent" field can reference it by index rather than by name (no
        // uniqueness requirement on GameObject::name this way). Scene::createGameObject
        // requires a parent to already exist before a child can be constructed with it,
        // so getGameObjects() is guaranteed to list every parent before its children -
        // a single forward pass is enough. Lights never participate in this map -
        // they have no parent concept.
        std::unordered_map<const GameObject *, size_t> indexOf;

        for (const auto &obj : scene.getGameObjects())
        {
            json node;
            node["type"] = "GameObject/" + obj->getTypeName();
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

            // Subclass-specific extra fields (e.g. a future GameObject subclass's own
            // data) go directly into 'node' alongside the base fields above - see
            // GameObject::onSerialize()'s doc comment for the reserved-key caveat.
            obj->onSerialize(node);

            indexOf[obj.get()] = root["entities"].size();
            root["entities"].push_back(std::move(node));
        }

        // Written after every GameObject, so a GameObject's "parent" index (an
        // absolute index into root["entities"]) always lands on another GameObject -
        // Lights are never a valid parent target and are never assigned one.
        for (const auto &light : scene.getLights())
        {
            json node;
            node["type"] = "Light/" + light->getTypeName();
            node["name"] = light->name;

            // Every field beyond type/name (color/intensity, plus whatever the
            // concrete type adds - position, direction, cone angles, ...) is the
            // light's own responsibility - SceneSerializer never mentions a concrete
            // Light subclass by name.
            light->onSerialize(node);

            root["entities"].push_back(std::move(node));
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

    bool SceneSerializer::load(Scene &outScene, const std::string &path, AssetManager &assetManager)
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

        if (!root.contains("entities") || !root["entities"].is_array())
        {
            Logging::Error("SceneSerializer::load - '" + path + "' has no 'entities' array");
            return false;
        }

        // Every GameObject is constructed top-level first (parent = nullptr);
        // parent/child links are resolved in a second pass below via the public
        // GameObject::setParent(), so a GameObjectFactory::CreateFn never needs to
        // reason about hierarchy. 'gameObjectByEntityIndex' maps each GameObject's
        // absolute position in root["entities"] to the object created for it, so a
        // "parent" field (also an absolute entities-array index, per save() above)
        // can be resolved with a plain lookup even if Light entities are interleaved
        // between GameObjects in the file. Light entities are never added here -
        // they have no parent to resolve, and can never be a "parent" target.
        std::unordered_map<size_t, GameObject *> gameObjectByEntityIndex;

        for (size_t i = 0; i < root["entities"].size(); ++i)
        {
            const json &node = root["entities"][i];

            std::string fullType = node.value("type", std::string("GameObject/GameObject"));
            EntityType entityType = splitEntityType(fullType);

            if (entityType.category == "Light")
            {
                Light *light = LightFactory::create(entityType.typeName, outScene, node);

                if (light == nullptr)
                {
                    Logging::Warning(
                        "SceneSerializer::load - unrecognized light type '" +
                        entityType.typeName + "', skipping entity");
                    continue;
                }

                light->name = node.value("name", std::string());

                // LightFactory's CreateFn already read every field this light type
                // needs (color/intensity plus its own concrete fields, see
                // LightFactory.cpp) - unlike GameObject, there's no base
                // transform/mesh/material to apply here, and no parent to resolve.
                continue;
            }

            if (entityType.category != "GameObject")
            {
                Logging::Warning(
                    "SceneSerializer::load - unrecognized entity category '" +
                    entityType.category + "' for type '" + fullType + "', skipping entity");
                continue;
            }

            GameObject *obj = GameObjectFactory::create(entityType.typeName, outScene, node, assetManager);

            if (obj == nullptr)
            {
                if (entityType.typeName != "GameObject")
                    Logging::Warning(
                        "SceneSerializer::load - unrecognized type '" + fullType +
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
                }
            }

            gameObjectByEntityIndex[i] = obj;
        }

        for (const auto &[entityIndex, obj] : gameObjectByEntityIndex)
        {
            const json &node = root["entities"][entityIndex];
            if (!node.contains("parent"))
                continue;

            size_t parentIndex = node["parent"].get<size_t>();

            auto it = gameObjectByEntityIndex.find(parentIndex);
            if (it == gameObjectByEntityIndex.end())
            {
                Logging::Warning(
                    "SceneSerializer::load - parent index " + std::to_string(parentIndex) +
                    " out of range for object '" + obj->name + "'");
                continue;
            }

            obj->setParent(it->second);
        }

        Logging::Info("SceneSerializer::load - loaded scene from " + path);
        return true;
    }

}
