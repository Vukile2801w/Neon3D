#include "Events/MouseScrolledEvent.hpp"

namespace Neon
{
    MouseScrolledEvent::MouseScrolledEvent(MouseScrolledEventData data) : m_data(data)
    {
    }

    MouseScrolledEvent::MouseScrolledEvent(const glm::vec2 &offset) : m_data{offset}
    {
    }

    EventType MouseScrolledEvent::getType() const
    {
        return EventType::MouseScrolled;
    }

    const char *MouseScrolledEvent::getName() const
    {
        return "MouseScrolledEvent";
    }

    const MouseScrolledEvent::MouseScrolledEventData &MouseScrolledEvent::getData() const
    {
        return m_data;
    }
}