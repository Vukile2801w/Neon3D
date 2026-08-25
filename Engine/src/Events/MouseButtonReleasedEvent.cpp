#include "Events/MouseButtonReleasedEvent.hpp"

namespace Neon
{
    MouseButtonReleasedEvent::MouseButtonReleasedEvent(MouseButtonReleasedEventData data) : m_data(data)
    {
    }

    MouseButtonReleasedEvent::MouseButtonReleasedEvent(int button) : m_data{button}
    {
    }

    EventType MouseButtonReleasedEvent::getType() const
    {
        return EventType::MouseButtonReleased;
    }

    const char *MouseButtonReleasedEvent::getName() const
    {
        return "MouseButtonReleasedEvent";
    }

    const MouseButtonReleasedEvent::MouseButtonReleasedEventData &MouseButtonReleasedEvent::getData() const
    {
        return m_data;
    }
}