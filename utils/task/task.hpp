#pragma once

#include <atomic>
#include <functional>

#include "common/computing.hpp"

namespace xlab {

class Task {
public:
    enum class Plan {
        CountingAndTicking,
        CountingOrTicking,
    };

public:
    Task(Counting::type maxCnt = Counting::type(1), Ticking::type maxTime = Ticking::type::Zero())
        : cnt(maxCnt)
        , tick(maxTime)
    {
    }

    ~Task() { reset(); }

    void setMaxRunCount(Counting::type maxCnt)
    {
        cnt.setMaxVal(maxCnt);
    }

    void setMaxRunTime(Ticking::type maxTime)
    {
        tick.setMaxVal(maxTime);
    }

    Counting::type getMaxRunCount() const
    {
        return cnt.getMaxVal();
    }

    Ticking::type getMaxRunTime() const
    {
        return tick.getMaxVal();
    }

    Counting::type getCurRunCount() const
    {
        return cnt.getVal();
    }

    Ticking::type getCurRunTime() const
    {
        return tick.getVal();
    }

    void pauseOn()
    {
        isPause = true;
    }

    void pauseOff()
    {
        isPause = false;
    }

    template <Plan PLAN = Plan::CountingAndTicking>
    bool run(const std::function<void(void)>& taskFunc = nullptr)
    {
        if (isPause) {
            return false;
        }

        bool done = false;
        if (PLAN == Plan::CountingAndTicking) {
            done = (cnt.done() && tick.done());
        } else if (PLAN == Plan::CountingOrTicking) {
            done = (cnt.done() || tick.done());
        }

        if (done) {
            return false;
        }

        if (!cnt.done()) {
            cnt.selfInc();
        }

        if (!tick.done()) {
            tick.selfInc();
        }

        if (PLAN == Plan::CountingAndTicking) {
            done = (cnt.done() && tick.done());
        } else if (PLAN == Plan::CountingOrTicking) {
            done = (cnt.done() || tick.done());
        }

        if (!done) {
            return false;
        }

        if (taskFunc != nullptr) {
            taskFunc();
        }

        return true;
    }

    bool run(const std::function<void(void)>& taskFunc = nullptr)
    {
        return run<>(taskFunc);
    }

    void reset()
    {
        cnt.reset();
        tick.reset();
        pauseOff();
    }

private:
    std::atomic<bool> isPause = false;
    Counting cnt { Counting::type(1) };
    Ticking tick { Ticking::type::Zero() };
};

}
