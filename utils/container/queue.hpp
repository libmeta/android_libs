#pragma once

#include <atomic>
#include <list>
#include <mutex>
#include <optional>
#include <queue>

namespace xlab {

template <typename E>
class Queue {
public:
    Queue() = default;

    ~Queue() = default;

    void Push(const E& element)
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        mQueue.push(element);
    }

    std::optional<E> Front()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        if (mQueue.empty()) {
            return std::nullopt;
        }
        return std::make_optional(mQueue.front());
    }

    std::optional<E> Pop()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        if (mQueue.empty()) {
            return std::nullopt;
        }

        auto front = std::make_optional(mQueue.front());
        mQueue.pop();
        return front;
    }

    /// 所有出队, 返回最后队尾数据
    std::optional<E> PopAll()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        if (mQueue.empty()) {
            return std::nullopt;
        }

        auto last = static_cast<E>(nullptr);
        while (!mQueue.empty()) {
            last = mQueue.front();
            mQueue.pop();
        }

        return std::make_optional(last);
    }

    /// 出队直至队尾, 返回队尾数据
    std::optional<E> PopUntilLast()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        if (mQueue.empty()) {
            return std::nullopt;
        }

        while (mQueue.size() > 1) {
            mQueue.pop();
        }

        return std::make_optional(mQueue.front());
    }

    void Clear()
    {
        std::lock_guard<decltype(mMutex)> locker(mMutex);
        mQueue = decltype(mQueue)();
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

private:
    std::mutex mMutex;
    std::queue<E, std::list<E>> mQueue;
};

}
