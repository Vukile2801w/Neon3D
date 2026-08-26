#include "Scene/Behavior.hpp"

#include "Assert.hpp"

namespace Neon
{
    Behavior::Behavior(GameObject *owner, Scene *root, Application *application)
        : m_gameObject(owner), m_scene(root), m_application(application)
    {
        NEON_ASSERT(m_gameObject != nullptr, "Behavior constructed with a null owning GameObject");
        NEON_ASSERT(m_scene != nullptr, "Behavior constructed with a null owning Scene");
        NEON_ASSERT(m_application != nullptr, "Behavior constructed with a null owning Application");
    }
}