#ifndef NEON_LIGHT
#define NEON_LIGHT

#include <string>

#include "glm.hpp"
#include "json_fwd.hpp"

namespace Neon
{
    class Scene;

    // Abstract base for scene lights. Deliberately separate from GameObject (see
    // Scene.hpp/SceneSerializer.hpp) - a Light is not a scene node with a
    // Transform/Mesh/Material, it's pure light data. Owned exclusively by Scene
    // (Scene::m_lights, a vector<unique_ptr<Light>> - see Scene::createLight()),
    // never by a GameObject and never referenced from Material.
    //
    // Only the data every light type actually shares lives here (color, intensity,
    // name, identity/lifecycle). Position/direction/cone angles etc. belong on the
    // concrete subclasses (PointLight/SpotLight/DirectionalLight) - not every light
    // has a position (DirectionalLight doesn't), so there is no position/direction
    // API on the base class.
    class Light
    {
    public:
        // Scene::createLight<T>() always constructs its Type with 'this' as the
        // first argument (the same convention Scene::createGameObject<T>() already
        // uses for 'this, parent') - every concrete Light type picks this up via
        // 'using Light::Light;' instead of repeating a constructor. 'scene' is the
        // owning Scene, non-owning here, mirroring GameObject::m_scene.
        explicit Light(Scene *scene) : m_scene(scene) {}

        virtual ~Light() = default;

        // Bare subclass name, no category prefix - mirrors GameObject::getTypeName()
        // ("Cube", not "GameObject/Cube"). SceneSerializer is the only place that
        // knows about the "Light/" prefix (see SceneSerializer.cpp) - it's the
        // router, this is just the type's own name. Pure virtual because, unlike
        // GameObject, Light is never itself a concrete/usable type.
        virtual std::string getTypeName() const = 0;

        // Called by SceneSerializer::save() after it has already written the base
        // fields ("type"/"name") for this light into 'out'. Every concrete Light
        // writes its own data here (including 'color'/'intensity' from this base
        // class - there's no default to inherit, since Light is never constructed
        // on its own). See PointLight/SpotLight/DirectionalLight for the reverse
        // (reading) side, which lives in each type's LightFactory-registered
        // CreateFn - the same read/write split GameObject already uses (read in
        // GameObjectFactory::CreateFn, write in GameObject::onSerialize()).
        virtual void onSerialize(nlohmann::json &out) const = 0;

        Scene *getScene() const { return m_scene; }

        // Optional label, purely for the scene file / debugging - mirrors
        // GameObject::name. Never required to be unique.
        std::string name;

        glm::vec3 color{1.0f};
        float intensity{1.0f};

        bool isPendingKill() const { return m_isPendingKill; }
        void markPendingKill() { m_isPendingKill = true; }

    private:
        Scene *m_scene{nullptr};
        bool m_isPendingKill{false};
    };
}

#endif
