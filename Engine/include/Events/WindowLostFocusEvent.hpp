#ifndef NEON_WINDOWLOSTFOCUSEVENT
#define NEON_WINDOWLOSTFOCUSEVENT

#include "Event.hpp"

namespace Neon
{
    class WindowLostFocusEvent : public Event
    {
    public:
        struct WindowLostFocusEventData
        {
        };

        WindowLostFocusEvent(WindowLostFocusEventData data = {});

        EventType getType() const override;
        const char *getName() const override;

        const WindowLostFocusEventData &getData() const;

    private:
        WindowLostFocusEventData m_data;
    };
}
#endif