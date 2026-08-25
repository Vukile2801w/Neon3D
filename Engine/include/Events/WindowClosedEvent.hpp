#ifndef NEON_WINDOWCLOSEDEVENT
#define NEON_WINDOWCLOSEDEVENT

#include "Event.hpp"

namespace Neon
{
    class WindowClosedEvent : public Event
    {
    public:
        struct WindowClosedEventData
        {
        };

        WindowClosedEvent(WindowClosedEventData data = {});

        EventType getType() const override;
        const char *getName() const override;

        const WindowClosedEventData &getData() const;

    private:
        WindowClosedEventData m_data;
    };
}
#endif