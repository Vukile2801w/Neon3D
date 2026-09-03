#ifndef NEON_GAME_OBJECT
#define NEON_GAME_OBJECT

#include <memory>
#include <vector>

#include "Rendering/Material.hpp"
#include "Scene/Behavior.hpp"
#include "Mesh/Mesh.hpp"
#include "Transform.hpp"

#include "glm.hpp"
#include "json_fwd.hpp"

namespace Neon
{
    class Scene;
    class Application;

    // A single node in the scene hierarchy. Not an ECS component/entity split -
    // GameObject is a plain composite of Transform + optional Mesh/Material + optional
    // Behavior, per the "simple GameObject list" decision over a full ECS.
    //
    // Ownership:
    //   - Mesh is a Ref<Mesh> (shared ownership) - GameObjects can share the same
    //     underlying Mesh without worrying about which one "owns" it.
    //   - Material* is non-owning - borrowed from wherever it's constructed
    //     (there's no resource manager wiring Material lifetime yet, see
    //     CONTRIBUTING §10).
    //   - Behavior is owned via std::unique_ptr (single owner: this GameObject).
    //   - parent/children are non-owning raw pointers - they describe the hierarchy,
    //     they don't control lifetime. Every GameObject's actual lifetime is owned by
    //     the Scene that created it (Scene::m_gameObjects), never by its parent.
    //   - A GameObject never deletes itself, its parent, or its children directly -
    //     destruction always goes through Scene::destroy() (cascades to children) so
    //     the flat owning list and the tree overlay never disagree about what's alive.
    class GameObject
    {
    public:
        // 'scene' is the owning Scene and must never be null - GameObjects are only
        // ever constructed by Scene::createGameObject(). 'parent' is optional
        // (nullptr = top-level object).
        explicit GameObject(Scene *scene, GameObject *parent = nullptr);
        ~GameObject();

        virtual std::string getTypeName()
        {
            return "GameObject";
        }

        // Called by SceneSerializer::save(), after it has already written the base
        // fields (type/name/transform/mesh/parent) into 'out'. A subclass overrides
        // this to add whatever *extra* fields its own GameObjectFactory-registered
        // constructor function will need back (e.g. Cube writes "isLightSource" and
        // "color"). Default does nothing, so a plain GameObject needs no override.
        // 'out' is the same JSON object representing this GameObject as a whole -
        // don't overwrite the reserved base keys ("type", "name", "transform",
        // "mesh", "parent"). See GameObjectFactory.hpp for the loading side.
        virtual void onSerialize(nlohmann::json &out) const {}

        // Local-space transform, relative to the parent (or to world space, for a
        // top-level object with no parent). Freely mutable, like Camera/Transform's own
        // convention - see getWorldMatrix() below for why that means no matrix caching.
        Transform transform;
        Ref<Mesh> mesh{nullptr};
        Material *material{nullptr};

        // Optional label, purely for the scene file / debugging - never required to
        // be unique, never used to resolve parent links (those use array indices,
        // see SceneSerializer.cpp).
        std::string name;

        // The AssetManager path 'mesh' was loaded from, if any. Set by whoever
        // assigns 'mesh' (e.g. Cube's constructor, or SceneSerializer::load() when a
        // scene file specifies a "mesh" path). Exists solely so SceneSerializer::save()
        // can write a mesh reference back out - there's no reverse path lookup from a
        // Ref<Mesh> through AssetManager today. Empty means "no mesh reference to save".
        std::string meshPath;

        template <typename T, typename... Args>
        void setBehavior(Args &&...args)
        {
            m_behavior = std::make_unique<T>(
                this,
                m_scene,
                m_application,
                std::forward<Args>(args)...);
        }
        Behavior *getBehavior() const;

        GameObject *getParent() const;
        const std::vector<GameObject *> &getChildren() const;

        // Detaches from the current parent (if any) and attaches to 'parent'
        // (nullptr = become/stay top-level). Keeps both sides of the relationship
        // (this object's m_parent and the parent's m_children) consistent.
        // 'parent' must not be this object or any of its own descendants - that would
        // create a cycle, which is a programmer error (see NEON_ASSERT in the .cpp),
        // not something legitimate usage can trigger.
        void setParent(GameObject *parent);

        Scene *getScene() const;

        // This object's model matrix in local space - i.e. transform.getMatrix() alone,
        // with no parent influence.
        glm::mat4 getLocalMatrix() const;

        // This object's model matrix in world space: its local matrix composed with
        // every ancestor's local matrix, walked live up the parent chain on every call.
        // Recomputed each time rather than cached - transform is a freely-mutable public
        // field (see above), so there's no reliable way to know it's gone stale without
        // wrapping it in a setter, which would give up that convention.
        glm::mat4 getWorldMatrix() const;

        // True once Scene::destroy() has marked this object (or an ancestor) for
        // removal. Pending-kill objects are skipped by Scene::update() and erased at
        // the end of that frame's update - see Scene.hpp.
        bool isPendingKill() const;

    private:
        friend class Scene;

        // Hierarchy bookkeeping used by setParent()/Scene - not part of the public API,
        // since calling these without keeping both sides in sync would break the
        // parent/children invariant.
        void addChild(GameObject *child);
        void removeChild(GameObject *child);

        void markPendingKill();

        Scene *m_scene{nullptr};
        Application *m_application;
        GameObject *m_parent{nullptr};
        std::vector<GameObject *> m_children;
        std::unique_ptr<Behavior> m_behavior;
        bool m_pendingKill{false};
    };
}

#endif