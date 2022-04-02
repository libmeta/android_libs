#pragma once

#include <optional>

#include "semaphore/semaphore.hpp"
#include "task/task_func.hpp"
#include "time/time_utils.hpp"

namespace xlab {

struct Counting {
    using type = int64_t;

    explicit Counting(type max)
        : maxVal(max)
    {
    }

    ~Counting() { reset(); }

    void setMaxVal(type max)
    {
        maxVal = max;
    }

    type getMaxVal() const
    {
        return maxVal;
    }

    static type zero()
    {
        return 0;
    }

    void selfInc()
    {
        ++val;
    }

    bool done() const
    {
        return val >= maxVal;
    }

    void reset()
    {
        val = zero();
    }

    type getVal() const
    {
        return val;
    }

private:
    volatile type val = zero();
    type maxVal = zero();
};

struct Ticking {
    using type = Time::Interval;

    explicit Ticking(type max)
        : maxTimeInterval(max)
    {
    }

    ~Ticking() { reset(); };

    void setMaxVal(type max)
    {
        maxTimeInterval = max;
    }

    type getMaxVal() const
    {
        return maxTimeInterval;
    }

    void selfInc()
    {
        if (!startTime.has_value()) {
            startTime = Time::Point::Now();
        }
        currentTimeInterval = Time::Point::Now() - startTime.value();
    }

    bool done() const
    {
        return currentTimeInterval >= maxTimeInterval;
    }

    void reset()
    {
        startTime = std::nullopt;
        currentTimeInterval = type::Zero();
    }

    type getVal() const
    {
        return currentTimeInterval;
    }

private:
    std::optional<Time::Point> startTime {};
    type currentTimeInterval {};
    type maxTimeInterval {};
};

}
