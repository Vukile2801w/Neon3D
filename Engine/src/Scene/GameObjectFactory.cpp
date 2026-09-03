#include "Scene/GameObjectFactory.hpp"

#include "Assert.hpp"
#include "Logging.hpp"

namespace Neon
{
    std::unordered_map<std::string, GameObjectFactory::CreateFn> &GameObjectFactory::registry()
    {
        static std::unordered_map<std::string, CreateFn> instance;
        return instance;
    }

    void GameObjectFactory::registerType(const std::string &typeName, CreateFn fn)
    {
        NEON_ASSERT(fn != nullptr, "GameObjectFactory::registerType called with a null constructor function");

        if (registry().count(typeName))
            Logging::Warning("GameObjectFactory: type '" + typeName + "' registered more than once, overwriting");

        registry()[typeName] = std::move(fn);

        Logging::Info("GameObjectFactory: registered type '" + typeName + "'");
    }

    GameObject *GameObjectFactory::create(
        const std::string &typeName,
        Scene &scene,
        const nlohmann::json &node,
        AssetManager &assetManager)
    {
        auto it = registry().find(typeName);
        if (it == registry().end())
            return nullptr;

        return it->second(scene, node, assetManager);
    }

    bool GameObjectFactory::isRegistered(const std::string &typeName)
    {
        return registry().count(typeName) != 0;
    }
}