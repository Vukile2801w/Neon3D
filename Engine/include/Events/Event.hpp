#ifndef NEON_EVENT
#define NEON_EVENT

namespace Neon
{
    enum class EventType
    {
        Unknown,

        // ===== Input ===== //
        MouseMoved,
        MouseButtonPressed,
        MouseButtonReleased,
        MouseScrolled,

        KeyPressed,
        KeyReleased,

        // ===== Window ===== //
        WindowClosed,
        WindowResized,
        WindowFocused,
        WindowLostFocus,

        EventCount
    };

    class Event
    {
    public:
        virtual ~Event() = default;

        virtual EventType getType() const = 0;
        virtual const char *getName() const = 0;
    };
}

#endif