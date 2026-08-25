#ifndef NEON_WINDOWFOCUSEDEVENT
#define NEON_WINDOWFOCUSEDEVENT

#include "Event.hpp"

namespace Neon
{
    class WindowFocusedEvent : public Event
    {
    public:
        struct WindowFocusedEventData
        {
        };

        WindowFocusedEvent(WindowFocusedEventData data = {});

        EventType getType() const override;
        const char *getName() const override;

        const WindowFocusedEventData &getData() const;

    private:
        WindowFocusedEventData m_data;
    };
}
#endif