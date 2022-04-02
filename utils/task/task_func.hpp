#pragma once

#include <functional>

#include "time/time_utils.hpp"

namespace xlab::TaskFunc {

static inline Time::Interval Loop(Time::Interval loopTime, std::function<void(void)> taskFunc)
{
    if (!taskFunc) {
        return Time::Interval::Zero();
    }

    const auto start = Time::Point::Now();
    for (; (Time::Point::Now() - start) <= loopTime;) {
        taskFunc();
    }

    return Time::Point::Now() - start;
}

static inline Time::Interval LoopUntil(Time::Interval maxLoopTime, std::function<bool(void)> taskFunc)
{
    if (!taskFunc) {
        return Time::Interval::Zero();
    }

    const auto start = Time::Point::Now();
    for (; Time::Point::Now() - start <= maxLoopTime;) {
        const auto quit = taskFunc();
        if (quit) {
            break;
        }
    }

    return Time::Point::Now() - start;
}

static inline Time::Interval Timing(std::function<void(void)> taskFunc)
{
    if (!taskFunc) {
        return Time::Interval::Zero();
    }

    const auto start = Time::Point::Now();
    taskFunc();
    return Time::Point::Now() - start;
}

}
