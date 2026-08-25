#include "Events/WindowFocusedEvent.hpp"

namespace Neon
{
    WindowFocusedEvent::WindowFocusedEvent(WindowFocusedEventData data) : m_data(data)
    {
    }

    EventType WindowFocusedEvent::getType() const
    {
        return EventType::WindowFocused;
    }

    const char *WindowFocusedEvent::getName() const
    {
        return "WindowFocusedEvent";
    }

    const WindowFocusedEvent::WindowFocusedEventData &WindowFocusedEvent::getData() const
    {
        return m_data;
    }
}