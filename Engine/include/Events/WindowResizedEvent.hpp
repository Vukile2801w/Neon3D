#ifndef NEON_WINDOWRESIZEDEVENT
#define NEON_WINDOWRESIZEDEVENT

#include "Event.hpp"

#include "glm.hpp"

namespace Neon
{
    class WindowResizedEvent : public Event
    {
    public:
        struct WindowResizedEventData
        {
            glm::vec2 size;
        };

        WindowResizedEvent(WindowResizedEventData data);
        WindowResizedEvent(const glm::vec2 &size);

        EventType getType() const override;
        const char *getName() const override;

        const WindowResizedEventData &getData() const;

    private:
        WindowResizedEventData m_data;
    };
}
#endif