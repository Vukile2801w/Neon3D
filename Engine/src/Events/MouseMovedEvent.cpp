#include "Events/MouseMovedEvent.hpp"

namespace Neon
{

    MouseMovedEvent::MouseMovedEvent(MouseMovedEventData data) : m_data(data)
    {
    }
    MouseMovedEvent::MouseMovedEvent(const glm::vec2 &pos, const glm::vec2 &delta) : m_data({pos, delta})
    {
    }

    const char *MouseMovedEvent::getName() const
    {
        return "MouseMovedEvent";
    }

    const MouseMovedEvent::MouseMovedEventData &MouseMovedEvent::getData() const
    {
        return m_data;
    }

    EventType MouseMovedEvent::getType() const
    {
        return EventType::MouseMoved;
    }
}