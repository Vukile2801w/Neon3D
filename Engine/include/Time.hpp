#ifndef NEON_TIME
#define NEON_TIME

#include <chrono>

namespace Neon
{
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    class Time
    {
    public:
        Time();

        TimePoint now() const;

        double sinceStart() const;
        double sinceStartOfFrame() const;
        double sinceLastFrame() const;

        float getDeltaTime() const;

        void beginFrame();

    private:
        TimePoint m_start;
        TimePoint m_startOfFrame;
        TimePoint m_endOfFrame;
    };
}

#endif