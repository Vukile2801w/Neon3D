#ifndef NEON_GAME_OBJECT_FACTORY
#define NEON_GAME_OBJECT_FACTORY

#include <functional>
#include <string>
#include <unordered_map>

#include "json_fwd.hpp"

namespace Neon
{
    class GameObject;
    class Scene;
    class AssetManager;

    // Lets a consumer (Sandbox) register how to construct one of its own GameObject
    // subclasses (e.g. Cube) from a scene-file JSON node, keyed by the string that
    // subclass's getTypeName() returns. This is what lets SceneSerializer (Engine-side)
    // build a Cube (Sandbox-side) without Engine ever including a Sandbox header -
    // Engine defines the interface (this class), Sandbox supplies the implementation
    // (the registered CreateFn). See CONTRIBUTING.md §7/§15.
    //
    // This is a registry of hand-written constructor functions, not a reflection
    // system - each CreateFn is ordinary code that knows exactly which JSON fields its
    // type needs and calls Scene::createGameObject<ConcreteType>(...) itself, the same
    // way Cube is already constructed manually in Sandbox/main.cpp today. See
    // CONTRIBUTING.md §13, "keep APIs small and intentional".
    class GameObjectFactory
    {
    public:
        // A CreateFn always constructs its object as top-level (no parent) -
        // SceneSerializer resolves parent/child links itself afterward via the public
        // GameObject::setParent(), so a CreateFn never needs to reason about hierarchy.
        // 'node' is the full JSON object SceneSerializer wrote for this GameObject
        // (including base fields like "transform" - a CreateFn only needs to read its
        // own extra fields; SceneSerializer::load() applies transform/mesh/name itself
        // after the object comes back).
        using CreateFn = std::function<GameObject *(
            Scene &scene,
            const nlohmann::json &node,
            AssetManager &assetManager)>;

        // Registers fn under typeName. Call once per type, before loading any scene
        // file that might reference it - e.g. at the top of Sandbox's
        // Application::onStart().
        static void registerType(const std::string &typeName, CreateFn fn);

        // Constructs a new instance of typeName via its registered CreateFn. Returns
        // nullptr if typeName was never registered - not an assert, since an
        // unrecognized type in a scene file is a data/runtime problem (old file, typo,
        // removed type), not a programmer error. See CONTRIBUTING.md §5/§6.
        static GameObject *create(
            const std::string &typeName,
            Scene &scene,
            const nlohmann::json &node,
            AssetManager &assetManager);

        static bool isRegistered(const std::string &typeName);

    private:
        // Function-local static registry avoids static-initialization-order issues if
        // registerType() is itself called from another translation unit's static
        // initialization (e.g. a self-registering helper run at program startup).
        static std::unordered_map<std::string, CreateFn> &registry();
    };
}

#endif