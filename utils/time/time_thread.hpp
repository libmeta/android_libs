#include <thread>

#include "time.hpp"

namespace std::this_thread {

static inline void sleep_for(const xlab::Time::Interval& interval)
{
    std::this_thread::sleep_for(interval.ToChrono<std::chrono::nanoseconds>());
}

static inline void sleep_until(const xlab::Time::Point& point)
{
    std::this_thread::sleep_until(point.ToStedyTimePoint());
}

}
