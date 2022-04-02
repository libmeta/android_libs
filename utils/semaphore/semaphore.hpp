#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "time/time_utils.hpp"

namespace xlab {

class Semaphore {
private:
    Semaphore(const Semaphore&) = delete;

    Semaphore& operator=(const Semaphore&) = delete;

    Semaphore& operator=(Semaphore&& sema) = delete;

    Semaphore(Semaphore&& sema) = delete;

public:
    Semaphore(int init_val = 0)
        : curCount(init_val)
    {
    }

    ~Semaphore() = default;

    void Wait()
    {
        while (true) {
            std::unique_lock<std::mutex> lock { mutex };
            if (curCount > 0) {
                --curCount;
                break;
            }
            condVar.wait(lock);
            if (curCount > 0) {
                --curCount;
                break;
            }
        }
    }

    bool TryWait(std::chrono::nanoseconds wait_time = std::chrono::nanoseconds::zero())
    {
        std::unique_lock<std::mutex> lock { mutex };
        if (curCount > 0) {
            --curCount;
            return true;
        }

        condVar.wait_for(lock, wait_time);
        if (curCount > 0) {
            --curCount;
            return true;
        }

        return false;
    }

    template <typename Rep, typename Period>
    bool TimedWait(const std::chrono::duration<Rep, Period>& duration)
    {
        auto nano = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        if (nano < std::chrono::nanoseconds::zero()) {
            return false;
        }
        std::unique_lock<std::mutex> lock { mutex };
        if (curCount > 0) {
            --curCount;
            return true;
        }

        condVar.wait_for(lock, nano);
        if (curCount > 0) {
            --curCount;
            return true;
        }

        return false;
    }

    bool TimedWait(const Time::Interval& interval)
    {
        return TimedWait(interval.ToChrono<std::chrono::nanoseconds>());
    }

    bool WaitUntil(const std::chrono::time_point<std::chrono::steady_clock>& abs_time)
    {
        std::unique_lock<std::mutex> lock { mutex };
        if (curCount > 0) {
            --curCount;
            return true;
        }

        condVar.wait_until(lock, abs_time);
        if (curCount > 0) {
            --curCount;
            return true;
        }

        return false;
    }

    bool WaitUntil(const Time::Point& point)
    {
        return WaitUntil(point.ToStedyTimePoint());
    }

    void Post(int count = 1)
    {
        if (count <= 0) {
            return;
        }
        std::lock_guard<std::mutex> lock { mutex };
        curCount += count;
        condVar.notify_one();
    }

    void ClearPost()
    {
        std::unique_lock<std::mutex> lock { mutex };
        if (curCount > 0) {
            curCount = 0;
        }
        condVar.wait_for(lock, std::chrono::nanoseconds::zero());
    }

private:
    int64_t curCount;
    std::mutex mutex;
    std::condition_variable condVar;
};

}
