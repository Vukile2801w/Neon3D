#include "Events/KeyReleasedEvent.hpp"

namespace Neon
{
    KeyReleasedEvent::KeyReleasedEvent(KeyReleasedEventData data) : m_data(data)
    {
    }

    KeyReleasedEvent::KeyReleasedEvent(int key) : m_data{key}
    {
    }

    EventType KeyReleasedEvent::getType() const
    {
        return EventType::KeyReleased;
    }

    const char *KeyReleasedEvent::getName() const
    {
        return "KeyReleasedEvent";
    }

    const KeyReleasedEvent::KeyReleasedEventData &KeyReleasedEvent::getData() const
    {
        return m_data;
    }
}