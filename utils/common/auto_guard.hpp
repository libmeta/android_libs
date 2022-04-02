#pragma once

#include <functional>

namespace xlab {

template <typename T, typename InitRet = void, typename DeinitRet = void>
struct auto_guard final {
    using InitFunction = std::function<InitRet(T& t)>;
    using DeinitFunction = std::function<DeinitRet(T& t)>;

private:
    T& t;

    InitRet* init_ret { nullptr };
    InitFunction init;

    DeinitRet* deinit_ret { nullptr };
    DeinitFunction deinit;

private:
    auto_guard(const auto_guard&) = delete;

    auto_guard(auto_guard&&) = delete;

    auto_guard& operator=(const auto_guard&) = delete;

    auto_guard& operator=(auto_guard&&) = delete;

public:
    explicit auto_guard(T& t, InitRet* init_ret, InitFunction init, DeinitRet* deinit_ret, DeinitFunction deinit)
        : t(t)
        , init_ret(init_ret)
        , init(std::move(init))
        , deinit_ret(deinit_ret)
        , deinit(std::move(deinit))
    {
        _call_init();
    }

    template <std::enable_if_t<std::is_same<void, InitRet>::value>* = nullptr>
    explicit auto_guard(T& t, InitFunction init, DeinitRet* deinit_ret, DeinitFunction deinit)
        : t(t)
        , init_ret(nullptr)
        , init(std::move(init))
        , deinit_ret(deinit_ret)
        , deinit(std::move(deinit))
    {
        _call_init();
    }

    template <std::enable_if_t<std::is_same<void, DeinitRet>::value>* = nullptr>
    explicit auto_guard(T& t, InitRet* init_ret, InitFunction init, DeinitFunction deinit)
        : t(t)
        , init_ret(init_ret)
        , init(std::move(init))
        , deinit_ret(nullptr)
        , deinit(std::move(deinit))
    {
        _call_init();
    }

    template <std::enable_if_t<std::is_same<void, InitRet>::value && std::is_same<void, DeinitRet>::value>* = nullptr>
    explicit auto_guard(T& t, InitFunction init, DeinitFunction deinit)
        : t(t)
        , init_ret(nullptr)
        , init(std::move(init))
        , deinit_ret(nullptr)
        , deinit(std::move(deinit))
    {
        _call_init();
    }

    ~auto_guard()
    {
        _call_deinit();
    }

public:
    DeinitFunction cancel_deinit()
    {
        DeinitFunction old_deinit_func = deinit;
        deinit = nullptr;
        return old_deinit_func;
    }

    void early_execute_deinit(bool clean = true)
    {
        _call_deinit();
        if (clean) {
            deinit = nullptr;
        }
    }

private:
    void _call_init()
    {
        if (!init) {
            return;
        }

        if constexpr (std::is_same<void, InitRet>::value) {
            init(t);
        } else {
            if (init_ret) {
                *(init_ret) = this->init(t);
            } else {
                init(t);
            }
        }
    }

    void _call_deinit()
    {
        if (!deinit) {
            return;
        }

        if constexpr (std::is_same<void, DeinitRet>::value) {
            deinit(t);
        } else {
            if (deinit_ret) {
                *(deinit_ret) = deinit(t);
            } else {
                deinit(t);
            }
        }
    }
}; // struct auto_guard<T>

struct auto_defer {
    using DeferFunction = std::function<void(void)>;

private:
    DeferFunction defer_function;

private:
    auto_defer(const auto_defer&) = delete;

    auto_defer(auto_defer&&) = delete;

    auto_defer& operator=(const auto_defer&) = delete;

    auto_defer& operator=(auto_defer&&) = delete;

public:
    auto_defer(DeferFunction defer_function)
        : defer_function(defer_function)
    {
    }

    ~auto_defer()
    {
        if (!defer_function) {
            return;
        }
        defer_function();
        defer_function = nullptr;
    }

public:
    DeferFunction cancel_defer()
    {
        DeferFunction old_defer_function = defer_function;
        defer_function = nullptr;
        return old_defer_function;
    }

    void early_execute(bool clean = true)
    {
        if (defer_function) {
            defer_function();
        }
        if (clean) {
            defer_function = nullptr;
        }
    }
}; // struct auto_defer

#define __COMBINE__(A, B) A##B
#define _COMBINE_(A, B) __COMBINE__(A, B)
#define defer(expr) __attribute__((unused)) xlab::auto_defer _COMBINE_(_defer_var_, __LINE__)(expr)

} // namespace xlab
