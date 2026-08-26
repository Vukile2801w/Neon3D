#include "Scene/GameObject.hpp"

#include <algorithm>

#include "Assert.hpp"
#include "Scene/Behavior.hpp"
#include "Logging.hpp"

#include "Scene/GameObject.hpp"
#include "Scene/Scene.hpp"

namespace Neon
{

    GameObject::GameObject(Scene *scene, GameObject *parent)
        : m_scene(scene),
          m_application(&scene->getApplication())
    {
        NEON_ASSERT(
            scene != nullptr,
            "GameObject constructed without an owning Scene");

        setParent(parent);

        Logging::Info("GameObject created");
    }

    // Defined here (not defaulted in the header) because std::unique_ptr<Behavior>'s
    // destructor needs Behavior's complete type, which is only visible after
    // including Behavior.hpp above.
    GameObject::~GameObject() = default;

    Behavior *GameObject::getBehavior() const
    {
        return m_behavior.get();
    }

    GameObject *GameObject::getParent() const
    {
        return m_parent;
    }

    const std::vector<GameObject *> &GameObject::getChildren() const
    {
        return m_children;
    }

    void GameObject::setParent(GameObject *parent)
    {
        if (m_parent == parent)
            return;

        for (GameObject *ancestor = parent; ancestor != nullptr; ancestor = ancestor->m_parent)
            NEON_ASSERT(ancestor != this, "setParent would create a cycle in the GameObject hierarchy");

        if (m_parent != nullptr)
            m_parent->removeChild(this);

        m_parent = parent;

        if (m_parent != nullptr)
            m_parent->addChild(this);
    }

    Scene *GameObject::getScene() const
    {
        return m_scene;
    }

    glm::mat4 GameObject::getLocalMatrix() const
    {
        return transform.getMatrix();
    }

    glm::mat4 GameObject::getWorldMatrix() const
    {
        if (m_parent != nullptr)
            return m_parent->getWorldMatrix() * getLocalMatrix();

        return getLocalMatrix();
    }

    bool GameObject::isPendingKill() const
    {
        return m_pendingKill;
    }

    void GameObject::addChild(GameObject *child)
    {
        m_children.push_back(child);
    }

    void GameObject::removeChild(GameObject *child)
    {
        auto it = std::find(m_children.begin(), m_children.end(), child);
        if (it != m_children.end())
            m_children.erase(it);
    }

    void GameObject::markPendingKill()
    {
        m_pendingKill = true;
    }
}