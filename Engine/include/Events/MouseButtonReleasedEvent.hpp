#ifndef NEON_MOUSEBUTTONRELEASEDEVENT
#define NEON_MOUSEBUTTONRELEASEDEVENT

#include "Event.hpp"

namespace Neon
{
    class MouseButtonReleasedEvent : public Event
    {
    public:
        struct MouseButtonReleasedEventData
        {
            int button;
        };

        MouseButtonReleasedEvent(MouseButtonReleasedEventData data);
        MouseButtonReleasedEvent(int button);

        EventType getType() const override;
        const char *getName() const override;

        const MouseButtonReleasedEventData &getData() const;

    private:
        MouseButtonReleasedEventData m_data;
    };
}
#endif