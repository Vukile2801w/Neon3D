#ifndef NEON_SCENE_SERIALIZER
#define NEON_SCENE_SERIALIZER

#include <string>

namespace Neon
{
    class Scene;
    class AssetManager;

    // Reads/writes a Scene to a JSON file. The file is a single "entities" array;
    // each entity's "type" is "<Category>/<TypeName>" ("GameObject/Cube",
    // "Light/Point", ...). SceneSerializer only ever looks at the category - it
    // strips the prefix and routes to GameObjectFactory or LightFactory, and never
    // mentions a concrete GameObject or Light subclass by name (see
    // GameObjectFactory.hpp / LightFactory.hpp). Base GameObject fields (type, name,
    // transform, mesh reference, hierarchy) are handled generically here for every
    // GameObject; a subclass's own extra fields go through
    // GameObject::onSerialize()/its GameObjectFactory::CreateFn. Base Light fields
    // (type, name) are handled generically here for every Light; a subclass's own
    // extra fields go through Light::onSerialize()/its LightFactory::CreateFn. See
    // CONTRIBUTING.md §7/§15.
    //
    // Lights have no parent/hierarchy concept, unlike GameObjects - SceneSerializer
    // never resolves a "parent" field for a Light entity.
    class SceneSerializer
    {
    public:
        // Writes scene to path as JSON. Returns false (and logs via Logging::Error) on
        // any failure to open/write the file - a runtime/environment failure, not a
        // programmer error, per CONTRIBUTING.md §5.
        static bool save(const Scene &scene, const std::string &path);

        // Reads path and populates outScene. assetManager is forwarded to each
        // GameObject's registered GameObjectFactory::CreateFn (most types need it to
        // (re)load their mesh/texture/shader assets) and is used directly here to
        // resolve each GameObject's own "mesh" field. Light entities go through
        // LightFactory instead, which doesn't take an AssetManager (see
        // LightFactory.hpp). Returns false on file/parse failure or if 'entities' is
        // missing/malformed.
        static bool load(Scene &outScene, const std::string &path, AssetManager &assetManager);
    };
}

#endif
