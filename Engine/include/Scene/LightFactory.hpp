#ifndef NEON_LIGHT_FACTORY
#define NEON_LIGHT_FACTORY

#include <functional>
#include <string>
#include <unordered_map>

#include "json_fwd.hpp"

namespace Neon
{
    class Light;
    class Scene;

    // Same registry/factory shape as GameObjectFactory (see GameObjectFactory.hpp),
    // scaled down to what Light types actually need. Keyed by a Light subclass's own
    // getTypeName() (e.g. "Point"), never by the "Light/" category prefix -
    // SceneSerializer owns that prefix and strips/adds it itself (see
    // SceneSerializer.cpp), the same way it does for GameObjectFactory's "GameObject/"
    // prefix.
    //
    // Unlike GameObjectFactory::CreateFn, a LightFactory::CreateFn does not take an
    // AssetManager& - no Light type today (Point/Spot/Directional) loads an asset.
    // If a future light type needs one (e.g. a light cookie texture), add the
    // parameter then; every registered CreateFn is hand-written, so the cost of
    // widening the signature later is small.
    //
    // A CreateFn also never deals with hierarchy/parenting - Lights don't have a
    // parent concept (see Scene.hpp/GameObject.hpp), so there's nothing for
    // SceneSerializer to resolve afterward the way it resolves GameObject "parent"
    // indices.
    class LightFactory
    {
    public:
        // 'node' is the full JSON object SceneSerializer wrote for this light
        // (already stripped of its "Light/" type prefix by the caller). A CreateFn
        // reads whatever fields its own concrete Light type needs directly from
        // 'node' and constructs it via Scene::createLight<T>() - the same
        // read-in-the-factory / write-in-onSerialize split GameObject already uses.
        using CreateFn = std::function<Light *(
            Scene &scene,
            const nlohmann::json &node)>;

        // Registers fn under typeName. Call once per type before loading any scene
        // file that might reference it.
        static void registerType(const std::string &typeName, CreateFn fn);

        // Constructs a new instance of typeName via its registered CreateFn. Returns
        // nullptr if typeName was never registered - not an assert, since an
        // unrecognized type in a scene file is a data/runtime problem, not a
        // programmer error (see CONTRIBUTING.md §5/§6).
        static Light *create(
            const std::string &typeName,
            Scene &scene,
            const nlohmann::json &node);

        static bool isRegistered(const std::string &typeName);

    private:
        // Function-local static registry, same reasoning as
        // GameObjectFactory::registry() (avoids static-initialization-order issues).
        static std::unordered_map<std::string, CreateFn> &registry();
    };
}

#endif
