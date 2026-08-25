#ifndef NEON_KEYPRESSEDEVENT
#define NEON_KEYPRESSEDEVENT

#include "Event.hpp"

namespace Neon
{
    class KeyPressedEvent : public Event
    {
    public:
        struct KeyPressedEventData
        {
            int key;
            bool repeat;
        };

        KeyPressedEvent(KeyPressedEventData data);
        KeyPressedEvent(int key, bool repeat);

        EventType getType() const override;
        const char *getName() const override;

        const KeyPressedEventData &getData() const;

    private:
        KeyPressedEventData m_data;
    };
}
#endif