#include "Events/WindowLostFocusEvent.hpp"

namespace Neon
{
    WindowLostFocusEvent::WindowLostFocusEvent(WindowLostFocusEventData data)
        : m_data(data)
    {
    }

    EventType WindowLostFocusEvent::getType() const
    {
        return EventType::WindowLostFocus;
    }

    const char *WindowLostFocusEvent::getName() const
    {
        return "WindowLostFocusEvent";
    }

    const WindowLostFocusEvent::WindowLostFocusEventData &WindowLostFocusEvent::getData() const
    {
        return m_data;
    }
}