#ifndef NEON_MOUSEBUTTONPRESSEDEVENT
#define NEON_MOUSEBUTTONPRESSEDEVENT

#include "Event.hpp"

namespace Neon
{
    class MouseButtonPressedEvent : public Event
    {
    public:
        struct MouseButtonPressedEventData
        {
            int button;
        };

        MouseButtonPressedEvent(MouseButtonPressedEventData data);
        MouseButtonPressedEvent(int button);

        EventType getType() const override;
        const char *getName() const override;

        const MouseButtonPressedEventData &getData() const;

    private:
        MouseButtonPressedEventData m_data;
    };
}
#endif