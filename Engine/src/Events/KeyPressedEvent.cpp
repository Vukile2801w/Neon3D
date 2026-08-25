#include "Events/KeyPressedEvent.hpp"

namespace Neon
{
    KeyPressedEvent::KeyPressedEvent(KeyPressedEventData data) : m_data(data)
    {
    }

    KeyPressedEvent::KeyPressedEvent(int key, bool repeat) : m_data{key, repeat}
    {
    }

    EventType KeyPressedEvent::getType() const
    {
        return EventType::KeyPressed;
    }

    const char *KeyPressedEvent::getName() const
    {
        return "KeyPressedEvent";
    }

    const KeyPressedEvent::KeyPressedEventData &KeyPressedEvent::getData() const
    {
        return m_data;
    }
}