#pragma once

#include <functional>
#include <list>
#include <optional>
#include <queue>
#include <shared_mutex>

#include "semaphore/semaphore.hpp"

namespace xlab {

template <typename E>
class BlockQueue {

public:
    using value_type = E;
    using reference = value_type&;
    using const_reference = const value_type&;

private:
    BlockQueue(const BlockQueue&) = delete;

    BlockQueue(BlockQueue&&) = delete;

    BlockQueue& operator=(const BlockQueue&) = delete;

    BlockQueue& operator=(BlockQueue&&) = delete;

public:
    BlockQueue(int max = 3)
        : mMaxCount(max)
        , mSemapIn(max)
        , mSemapOut(0)
    {
    }

    ~BlockQueue() = default;

    bool TryPush(const E& element)
    {
        if (mSemapIn.TryWait()) {
            std::lock_guard<decltype(mMutex)> locker(mMutex);
            mQueue.push(element);
            mSemapOut.Post();
            return true;
        }
        return false;
    }

    void Push(const E& element)
    {
        mSemapIn.Wait();
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        mQueue.push(element);
        mSemapOut.Post();
    }

    void Pulse()
    {
        mSemapIn.Post();
        mSemapOut.Post();
    }

    void Reset()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        mSemapIn.ClearPost();
        mSemapOut.ClearPost();
        mQueue = decltype(mQueue)();
        mSemapIn.Post(mMaxCount);
    }

    std::optional<E> TryPop(const Time::Interval& wait_time)
    {
        return TryPop(wait_time.template ToChrono<ns>());
    }

    std::optional<E> TryPop(const std::chrono::nanoseconds& wait_time = std::chrono::nanoseconds::zero())
    {
        if (mSemapOut.TryWait(wait_time)) {
            std::optional<E> front = std::nullopt;
            std::lock_guard<decltype(mMutex)> lock(mMutex);
            if (mQueue.empty()) {
                return front;
            }
            front = std::make_optional(mQueue.front());
            mQueue.pop();
            mSemapIn.Post();
            return front;
        }
        return std::nullopt;
    }

    void Clear()
    {
        auto ret = TryPop();
        while (ret.has_value()) {
            ret = TryPop();
        }
    }

    void ClearIf(std::function<bool(E&)> func)
    {
        std::lock_guard<decltype(mMutex)> lock(mMutex);
        decltype(mQueue) newQueue;
        while (!mQueue.empty()) {
            auto element = mQueue.front();
            mQueue.pop();
            if (!func(element)) {
                newQueue.push(element);
            }
        }
        mQueue = newQueue;
    }

    std::optional<E> Pop()
    {
        mSemapOut.Wait();
        std::lock_guard<decltype(mMutex)> lock(mMutex);
        if (mQueue.empty()) {
            mSemapIn.Post();
            return std::nullopt;
        }
        auto front = std::make_optional(mQueue.front());
        mQueue.pop();
        mSemapIn.Post();
        return front;
    }

    std::optional<E> Front()
    {
        std::lock_guard<decltype(mMutex)> lock(mMutex);
        if (mQueue.empty()) {
            return std::nullopt;
        }
        return std::make_optional(mQueue.front());
    }

    std::optional<E> Back()
    {
        std::lock_guard<decltype(mMutex)> lock(mMutex);
        if (mQueue.empty()) {
            return std::nullopt;
        }
        return std::make_optional(mQueue.back());
    }

    size_t Size()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        return mQueue.size();
    }

    bool Empty()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        return mQueue.empty();
    }

    bool Full()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        return mQueue.size() >= mMaxCount;
    }

private:
    int mMaxCount;
    Semaphore mSemapIn;
    Semaphore mSemapOut;
    std::shared_mutex mMutex;
    std::queue<E, std::list<E>> mQueue;
};

}
