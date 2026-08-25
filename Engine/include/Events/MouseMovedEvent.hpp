#ifndef NEON_MOUSEMOVEDEVENT
#define NEON_MOUSEMOVEDEVENT

#include "Event.hpp"

#include "glm.hpp"

namespace Neon
{
    class MouseMovedEvent : public Event
    {
    public:
        struct MouseMovedEventData
        {
            glm::vec2 pos;
            glm::vec2 delta;
        };

        MouseMovedEvent(MouseMovedEventData data);
        MouseMovedEvent(const glm::vec2 &pos, const glm::vec2 &delta);

        EventType getType() const override;
        const char *getName() const override;

        const MouseMovedEventData &getData() const;

    private:
        MouseMovedEventData m_data;
    };
}

#endif