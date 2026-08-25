#include "Events/WindowClosedEvent.hpp"

namespace Neon
{
    WindowClosedEvent::WindowClosedEvent(WindowClosedEventData data) : m_data(data)
    {
    }

    EventType WindowClosedEvent::getType() const
    {
        return EventType::WindowClosed;
    }

    const char *WindowClosedEvent::getName() const
    {
        return "WindowClosedEvent";
    }

    const WindowClosedEvent::WindowClosedEventData &WindowClosedEvent::getData() const
    {
        return m_data;
    }
}