#pragma once

#include <memory>
#include <type_traits>

#include "common/global_function.hpp"
#include "sp_holder/sp_holder_recorder.hpp"
#include "common/template.hpp"

namespace xlab {

template <typename T>
class SPHolder final {
public:
    template <typename R, std::enable_if_t<std::is_same_v<T, R> || std::is_base_of_v<T, R>>* = nullptr>
    static SPHolder<T>* New(std::shared_ptr<R>& ptr) noexcept
    {
        static_assert(std::is_base_of<T, R>::value || std::is_same<T, R>::value, "ptr's type `R` not same `T` or not derived from `T`");
        return new SPHolder<T>(ptr);
    }

    template <typename R, std::enable_if_t<std::is_same_v<T, R> || std::is_base_of_v<T, R>>* = nullptr>
    static SPHolder<T>* New(std::shared_ptr<R>&& ptr) noexcept
    {
        static_assert(std::is_base_of<T, R>::value || std::is_same<T, R>::value, "ptr's type `R` not same `T` or not derived from `T`");
        return new SPHolder<T>(std::move(ptr));
    }

    static void Delete(SPHolder<T>* holder) noexcept
    {
        delete holder;
    }

    static void DeleteFromHandler(int64_t handler) noexcept
    {
        delete SPHolder<T>::FromHandler(handler);
    }

private:
    template <typename R, std::enable_if_t<std::is_same_v<T, R> || std::is_base_of_v<T, R>>* = nullptr>
    SPHolder(std::shared_ptr<R>& ptr) noexcept
        : mPtr(std::is_same_v<T, R> ? ptr : std::static_pointer_cast<T>(ptr))
    {
#if USING_SPHOLDER_RECORDER
        static std::string rName = GlobalFunc::demangle(typeid(R).name());
        SPHolderRecorder::Add(__PRETTY_FUNCTION__, rName, mPtr.get());
#endif
    }

    template <typename R, std::enable_if_t<std::is_same_v<T, R> || std::is_base_of_v<T, R>>* = nullptr>
    SPHolder(std::shared_ptr<R>&& ptr) noexcept
        : mPtr(std::move(std::is_same_v<T, R> ? ptr : std::static_pointer_cast<T>(ptr)))
    {
#if USING_SPHOLDER_RECORDER
        static std::string rName = GlobalFunc::demangle(typeid(R).name());
        SPHolderRecorder::Add(__PRETTY_FUNCTION__, rName, mPtr.get());
#endif
    }

    virtual ~SPHolder() noexcept
    {
#if USING_SPHOLDER_RECORDER
        SPHolderRecorder::Remove(__PRETTY_FUNCTION__, mPtr.get());
#endif
        mPtr = nullptr;
    }

public:
    static SPHolder<T>* FromHandler(int64_t handler) noexcept
    {
        return reinterpret_cast<SPHolder<T>*>(handler);
    }

    static std::shared_ptr<T> UnwrapFromHandler(int64_t handler) noexcept
    {
        auto holder = reinterpret_cast<SPHolder<T>*>(handler);
        if (!holder) {
            return nullptr;
        }
        return holder->Unwrap();
    }

    static int64_t NullHandler()
    {
        return 0;
    }

private:
    SPHolder(const SPHolder&) = delete;

    SPHolder(SPHolder&&) = delete;

    SPHolder& operator=(SPHolder&) = delete;

    SPHolder& operator=(SPHolder&&) = delete;

public:
    std::shared_ptr<T>& Unwrap() noexcept
    {
        return mPtr;
    }

    int64_t ToHandler()
    {
        return reinterpret_cast<int64_t>(this);
    }

private:
    std::shared_ptr<T> mPtr;
};

}

