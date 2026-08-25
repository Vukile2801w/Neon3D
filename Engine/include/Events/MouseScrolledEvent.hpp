#ifndef NEON_MOUSESCROLLEDEVENT
#define NEON_MOUSESCROLLEDEVENT

#include "Event.hpp"

#include "glm.hpp"

namespace Neon
{
    class MouseScrolledEvent : public Event
    {
    public:
        struct MouseScrolledEventData
        {
            glm::vec2 offset;
        };

        MouseScrolledEvent(MouseScrolledEventData data);
        MouseScrolledEvent(const glm::vec2 &offset);

        EventType getType() const override;
        const char *getName() const override;

        const MouseScrolledEventData &getData() const;

    private:
        MouseScrolledEventData m_data;
    };
}
#endif