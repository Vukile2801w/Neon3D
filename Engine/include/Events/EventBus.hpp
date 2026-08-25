#ifndef NEON_EVENTBUS
#define NEON_EVENTBUS

#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "Events/Event.hpp"

namespace Neon
{
    class EventBus
    {
    public:
        template <typename T>
        using EventCallback = std::function<void(const T &)>;

        template <typename T>
        void subscribe(EventCallback<T> callback);

        template <typename T>
        void emit(const T &event);

    private:
        using Callback = std::function<void(const Event &)>;

        std::unordered_map<
            std::type_index,
            std::vector<Callback>>
            m_callbacks;
    };

    // IMPLEMENTATION //

    template <typename T>
    void EventBus::subscribe(EventCallback<T> callback)
    {
        auto &callbacks = m_callbacks[typeid(T)];

        callbacks.emplace_back(
            [callback = std::move(callback)](const Event &event)
            {
                callback(static_cast<const T &>(event));
            });
    }

    template <typename T>
    void EventBus::emit(const T &event)
    {
        auto it = m_callbacks.find(typeid(T));

        if (it == m_callbacks.end())
            return;

        for (auto &callback : it->second)
            callback(event);
    }
}

#endif