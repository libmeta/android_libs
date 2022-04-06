#pragma once

#include <functional>
#include <string>
#include <thread>

#include "thread_unix.hpp"
#include "thread_win.hpp"

class ThreadWrap {
private:
    ThreadWrap(const ThreadWrap&) = delete;
    ThreadWrap& operator=(const ThreadWrap&) = delete;

public:
    using ThreadRunnable = std::function<void()>;

    ThreadWrap() = default;

    ThreadWrap(std::string name, ThreadRunnable runnable)
        : thread_name_(name)
    {
        run_ = [runnable, name]() {
            SetThreadName(name);
            if (runnable) {
                runnable();
            }
        };

        thread_ = std::thread(run_);
    }

    ThreadWrap(ThreadWrap&&) noexcept;

    ThreadWrap& operator=(ThreadWrap&& rhs) noexcept
    {
        Terminate();
        thread_ = std::move(rhs.thread_);
        thread_name_ = std::move(rhs.thread_name_);
        std::swap(run_, rhs.run_);
        return *this;
    }

    virtual ~ThreadWrap()
    {
        Terminate();
    }

    virtual void Terminate()
    {
        if (thread_.joinable()) {
            thread_.join();
        }
    }

    bool IsValid() const
    {
        return thread_.joinable();
    }

private:
    std::function<void()> run_;
    std::thread thread_;
    std::string thread_name_;
};
