#include "Events/MouseButtonPressedEvent.hpp"

namespace Neon
{
    MouseButtonPressedEvent::MouseButtonPressedEvent(MouseButtonPressedEventData data) : m_data(data)
    {
    }

    MouseButtonPressedEvent::MouseButtonPressedEvent(int button) : m_data{button}
    {
    }

    EventType MouseButtonPressedEvent::getType() const
    {
        return EventType::MouseButtonPressed;
    }

    const char *MouseButtonPressedEvent::getName() const
    {
        return "MouseButtonPressedEvent";
    }

    const MouseButtonPressedEvent::MouseButtonPressedEventData &MouseButtonPressedEvent::getData() const
    {
        return m_data;
    }
}