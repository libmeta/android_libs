#pragma once

#include <atomic>
#include <functional>
#include <thread>

#include "common/macro_conf.hpp"
#include "semaphore/semaphore.hpp"
#include "time/time_utils.hpp"

class TaskThread {
    IMPORT_CONSTRUCTOR_DELETE(TaskThread)
    TaskThread() = delete;

public:
    using FuncType = std::function<bool(void)>;
    explicit TaskThread(xlab::Time::Interval run_time, xlab::Time::Interval run_steup_time, const FuncType& run_func)
        : run_time_(run_time)
        , run_steup_time_(run_steup_time)
        , run_func_(run_func)
    {
        task_thread_ = std::thread([this] {
            if (run_func_ == nullptr || is_stop_) {
                return;
            }

            using namespace xlab::Time;
            for (const auto start = Point::Now(); !is_stop_ && Point::Now() - start <= run_time_;) {
                nap_steup_.TimedWait(run_steup_time_);
                if (run_func_()) {
                    run_ok_ = true;
                    break;
                }
            }
        });
    }

    ~TaskThread()
    {
        stop();
    }

    void stop()
    {
        is_stop_ = true;
        nap_steup_.Post();
        if (task_thread_.joinable()) {
            task_thread_.join();
        }
    }

    bool isEnd() const
    {
        return !task_thread_.joinable();
    }

    bool isRunOk() const
    {
        return run_ok_;
    }

private:
    FuncType run_func_;
    std::thread task_thread_;
    std::atomic<bool> is_stop_ = false;
    std::atomic<bool> run_ok_ = false;
    xlab::Semaphore nap_steup_;
    xlab::Time::Interval run_steup_time_;
    xlab::Time::Interval run_time_;
};
