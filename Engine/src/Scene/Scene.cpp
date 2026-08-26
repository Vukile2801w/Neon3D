#include "Scene/Scene.hpp"

#include <algorithm>

#include "Scene/Behavior.hpp"
#include "Scene/GameObject.hpp"
#include "Logging.hpp"
#include "Assert.hpp"

namespace Neon
{
    Scene::Scene(Application *application)
        : m_application(application)
    {
        NEON_ASSERT(
            application != nullptr,
            "Scene constructed without an Application");

        Logging::Info("Scene created");
    }

    // Defined here (not defaulted in the header) because std::unique_ptr<GameObject>'s
    // destructor needs GameObject's complete type.
    Scene::~Scene() = default;

    void Scene::destroy(GameObject *gameObject)
    {
        if (gameObject == nullptr || gameObject->isPendingKill())
            return;

        markSubtreePendingKill(gameObject);
    }

    void Scene::markSubtreePendingKill(GameObject *gameObject)
    {
        gameObject->markPendingKill();

        for (GameObject *child : gameObject->getChildren())
            markSubtreePendingKill(child);
    }

    void Scene::update(float dt)
    {
        for (auto &gameObject : m_gameObjects)
        {
            if (gameObject->isPendingKill())
                continue;

            Behavior *behavior = gameObject->getBehavior();
            if (behavior)
                behavior->update(dt);
        }

        sweepPendingKill();
    }

    void Scene::sweepPendingKill()
    {
        // Detach every about-to-be-erased object from a surviving parent first, so no
        // parent is left holding a dangling child pointer after erase() below. If the
        // parent is also pending-kill, it's about to be erased too, so there's nothing
        // to keep consistent there.
        for (auto &gameObject : m_gameObjects)
        {
            if (!gameObject->isPendingKill())
                continue;

            GameObject *parent = gameObject->getParent();
            if (parent != nullptr && !parent->isPendingKill())
                gameObject->setParent(nullptr);
        }

        m_gameObjects.erase(
            std::remove_if(m_gameObjects.begin(), m_gameObjects.end(),
                           [](const std::unique_ptr<GameObject> &gameObject)
                           {
                               return gameObject->isPendingKill();
                           }),
            m_gameObjects.end());
    }

    const std::vector<std::unique_ptr<GameObject>> &Scene::getGameObjects() const
    {
        return m_gameObjects;
    }
}