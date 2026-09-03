#ifndef NEON_SCENE_SERIALIZER
#define NEON_SCENE_SERIALIZER

#include <string>

namespace Neon
{
    class Scene;
    class AssetManager;
    class Renderer;

    // Reads/writes a Scene to a JSON file. Base GameObject fields (type, name,
    // transform, mesh reference, hierarchy) are handled generically here for every
    // object. A subclass's own extra fields (e.g. Cube's isLightSource/color) are
    // handled on the way out by that subclass's GameObject::onSerialize() override
    // (see GameObject.hpp), and on the way in by its own GameObjectFactory-registered
    // CreateFn (see GameObjectFactory.hpp). SceneSerializer itself never mentions a
    // concrete GameObject subclass by name - see CONTRIBUTING.md §7/§15.
    class SceneSerializer
    {
    public:
        // Writes scene to path as JSON. Returns false (and logs via Logging::Error) on
        // any failure to open/write the file - a runtime/environment failure, not a
        // programmer error, per CONTRIBUTING.md §5.
        static bool save(const Scene &scene, const std::string &path);

        // Reads path and populates outScene. assetManager is forwarded to each
        // object's registered GameObjectFactory::CreateFn (most types need it to
        // (re)load their mesh/texture/shader assets, the same way Cube's constructor
        // already does today) and is used directly here to resolve each object's own
        // "mesh" field. Returns false on file/parse failure or if 'objects' is
        // missing/malformed.
        static bool load(Scene &outScene, const std::string &path, AssetManager &assetManager, Renderer &renderer);
    };
}

#endif