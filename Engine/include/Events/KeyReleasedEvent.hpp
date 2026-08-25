#ifndef NEON_KEYRELEASEDEVENT
#define NEON_KEYRELEASEDEVENT

#include "Event.hpp"

namespace Neon
{
    class KeyReleasedEvent : public Event
    {
    public:
        struct KeyReleasedEventData
        {
            int key;
        };

        KeyReleasedEvent(KeyReleasedEventData data);
        KeyReleasedEvent(int key);

        EventType getType() const override;
        const char *getName() const override;

        const KeyReleasedEventData &getData() const;

    private:
        KeyReleasedEventData m_data;
    };
}
#endif