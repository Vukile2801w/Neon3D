#include "Time.hpp"

namespace Neon
{
    Time::Time()
        : m_start(Clock::now()),
          m_startOfFrame(m_start),
          m_endOfFrame(m_start)
    {
    }

    TimePoint Time::now() const
    {
        return Clock::now();
    }

    double Time::sinceStart() const
    {
        return std::chrono::duration<double>(now() - m_start)
            .count();
    }

    double Time::sinceStartOfFrame() const
    {
        return std::chrono::duration<double>(now() - m_startOfFrame)
            .count();
    }

    double Time::sinceLastFrame() const
    {
        return std::chrono::duration<double>(now() - m_endOfFrame)
            .count();
    }

    void Time::beginFrame()
    {
        m_endOfFrame = m_startOfFrame;
        m_startOfFrame = now();
    }

    float Time::getDeltaTime() const
    {
        return std::chrono::duration<float>(
                   m_startOfFrame - m_endOfFrame)
            .count();
    }
}