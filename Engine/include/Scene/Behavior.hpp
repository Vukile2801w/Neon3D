#ifndef NEON_BEHAVIOR
#define NEON_BEHAVIOR

namespace Neon
{
    class GameObject;
    class Scene;
    class Application;
    // Base class for per-GameObject gameplay logic (NOT an ECS system - this is plain
    // composition, one Behavior instance per GameObject that has one).
    //
    // A Behavior is owned by exactly one GameObject (attach it via
    // GameObject::setBehavior(std::make_unique<MyBehavior>(owner, scene))). 'owner' and
    // 'root' are non-owning and set once at construction - both are guaranteed valid
    // for the Behavior's entire lifetime, since the GameObject/Scene must already exist
    // to be passed in.
    class Behavior
    {
    public:
        Behavior(GameObject *owner, Scene *root, Application *application);
        virtual ~Behavior() = default;

        virtual void update(float dt) = 0;

    protected:
        // non-owning, set at construction, never null - see class comment
        GameObject *m_gameObject{nullptr};
        Scene *m_scene{nullptr};
        Application *m_application{nullptr};
    };
}

#endif