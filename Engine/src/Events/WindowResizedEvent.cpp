#include "Events/WindowResizedEvent.hpp"

namespace Neon
{
    WindowResizedEvent::WindowResizedEvent(WindowResizedEventData data) : m_data(data)
    {
    }

    WindowResizedEvent::WindowResizedEvent(const glm::vec2 &size) : m_data{size}
    {
    }

    EventType WindowResizedEvent::getType() const
    {
        return EventType::WindowResized;
    }

    const char *WindowResizedEvent::getName() const
    {
        return "WindowResizedEvent";
    }

    const WindowResizedEvent::WindowResizedEventData &WindowResizedEvent::getData() const
    {
        return m_data;
    }
}