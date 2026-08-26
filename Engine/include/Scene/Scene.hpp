#ifndef NEON_SCENE
#define NEON_SCENE

#include <memory>
#include <vector>

#include "Scene/GameObject.hpp"

namespace Neon
{
    class Application;

    // Owns every GameObject in the scene as a single flat vector<unique_ptr<GameObject>>.
    // The parent/child tree on GameObject (see GameObject.hpp) is a non-owning overlay
    // describing relationships only - Scene is the one and only place lifetime is
    // decided, so pointers into the hierarchy stay valid regardless of tree shape.
    class Scene
    {
    public:
        explicit Scene(Application *application);
        ~Scene();

        // Creates a new GameObject owned by this Scene and returns a non-owning
        // pointer to it. 'parent' is optional (nullptr = top-level object).
        template <typename Type, typename... Args>
        Type *createGameObject(GameObject *parent, Args &&...args)
        {
            auto gameObject = std::make_unique<Type>(
                this,
                parent,
                std::forward<Args>(args)...);

            Type *result = gameObject.get();

            m_gameObjects.push_back(std::move(gameObject));

            return result;
        }

        // Marks 'gameObject' and its entire subtree (recursively) as pending-kill.
        // Does NOT erase anything immediately - safe to call from inside a
        // Behavior::update(), including a Behavior destroying its own owner or a
        // sibling, without invalidating the iteration update() is currently doing.
        // Actual removal happens at the end of the current/next update() call.
        void destroy(GameObject *gameObject);

        // Calls Behavior::update(dt) on every non-pending-kill GameObject that has a
        // Behavior attached, then sweeps every GameObject marked pending-kill (by
        // destroy(), during this call or an earlier one) out of the scene.
        void update(float dt);

        const std::vector<std::unique_ptr<GameObject>> &getGameObjects() const;

        Application &getApplication()
        {
            return *m_application;
        }

    private:
        void markSubtreePendingKill(GameObject *gameObject);
        void sweepPendingKill();

        std::vector<std::unique_ptr<GameObject>> m_gameObjects;
        Application *m_application;
    };
}

#endif