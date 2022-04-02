#pragma once

#include <chrono>

namespace xlab {

namespace Time {

    class Interval;

    struct Point final {
    private:
        using value_type = std::chrono::steady_clock::time_point;
        value_type value {};

    public:
        explicit constexpr Point(const value_type& timePoint = value_type {}) noexcept;

        template <typename Rep, typename Period>
        constexpr Point(const std::chrono::duration<Rep, Period>& td) noexcept
            : value(std::chrono::steady_clock::time_point(td))
        {
        }

        template <typename Duration>
        constexpr Point(const std::chrono::time_point<value_type::clock, Duration>& timePoint) noexcept
            : value(std::chrono::time_point_cast<value_type::clock, value_type::duration>(timePoint))
        {
        }

        explicit constexpr Point(const Time::Interval& interval) noexcept;

    public:
        constexpr auto& Value() const noexcept
        {
            return value;
        }

    public:
        template <typename ChronoInterval>
        constexpr Point FromRawValue(int64_t rawValue) noexcept
        {
            return std::move(Point(ChronoInterval(rawValue)));
        }

    public:
        static const Point Now() noexcept;

        static constexpr Point Min() noexcept;

        static constexpr Point Max() noexcept;

    public:
        constexpr std::chrono::steady_clock::time_point ToStedyTimePoint() const noexcept;

        constexpr Point After(const Interval& timeInterval) const;

        constexpr Point Before(const Interval& timeInterval) const;

        const Interval ToInterval() const noexcept;

        template <typename ChronoInterval>
        const int64_t RawValue() const noexcept
        {
            if constexpr (std::is_same_v<ChronoInterval, value_type::duration>) {
                return static_cast<int64_t>(value.time_since_epoch().count());
            }
            return static_cast<int64_t>(std::chrono::duration_cast<ChronoInterval>(value.time_since_epoch()).count());
        }

    public:
        constexpr Point& operator+=(const Interval& d);

        constexpr Point& operator-=(const Interval& d);
    };

    struct Interval final {
    private:
        using value_type = std::chrono::nanoseconds;
        value_type value {};

    public:
        explicit constexpr Interval(const value_type& td = value_type {}) noexcept
            : value(td)
        {
        }

        template <typename Rep, typename Period>
        explicit constexpr Interval(const std::chrono::duration<Rep, Period>& td) noexcept
            : value(std::chrono::duration_cast<std::chrono::nanoseconds>(td))
        {
        }

    public:
        constexpr auto& Value() const noexcept
        {
            return value;
        }

    public:
        template <typename ChronoInterval>
        static const Interval FromRawValue(int64_t rawValue) noexcept
        {
            return std::move(Interval(ChronoInterval(rawValue)));
        }

    public:
        static const Interval Zero() noexcept;

        static const Interval Min() noexcept;

        static const Interval Max() noexcept;

    public:
        template <typename ChronoInterval>
        constexpr ChronoInterval ToChrono() const noexcept
        {
            if constexpr (std::is_same_v<ChronoInterval, decltype(value)>) {
                return value;
            }
            return std::chrono::duration_cast<ChronoInterval>(value);
        }

        constexpr Point ToPoint() const noexcept;

        template <typename ChronoInterval>
        constexpr int64_t RawValue() const noexcept
        {
            return static_cast<int64_t>(ToChrono<ChronoInterval>().count());
        }

    public:
        constexpr Interval& operator+=(const Interval& d);

        constexpr Interval& operator-=(const Interval& d);
    };

#pragma mark - Interval Functions

    const Interval Interval::Zero() noexcept
    {
        return std::move(Interval(value_type::zero()));
    }

    const Interval Interval::Min() noexcept
    {
        return std::move(Interval(value_type::min()));
    }

    const Interval Interval::Max() noexcept
    {
        return std::move(Interval(value_type::max()));
    }

    constexpr Point Interval::ToPoint() const noexcept
    {
        return Point(value);
    }

    constexpr Interval& Interval::operator+=(const Interval& d)
    {
        this->value += d.value;
        return *this;
    }

    constexpr Interval& Interval::operator-=(const Interval& d)
    {
        this->value -= d.value;
        return *this;
    }

#pragma mark - Point Functions

    constexpr Point::Point(const value_type& timePoint) noexcept
        : value(timePoint)
    {
    }

    constexpr Point::Point(const Time::Interval& interval) noexcept
        : value(std::chrono::steady_clock::time_point(interval.Value()))
    {
    }

    const Point Point::Now() noexcept
    {
        return std::move(Point(std::chrono::steady_clock::now()));
    }

    constexpr Point Point::Min() noexcept
    {
        return std::move(Point(value_type::min()));
    }

    constexpr Point Point::Max() noexcept
    {
        return std::move(Point(value_type::max()));
    }

    constexpr std::chrono::steady_clock::time_point Point::ToStedyTimePoint() const noexcept
    {
        return value;
    }

    constexpr Point Point::After(const Interval& timeInterval) const
    {
        return Point(value + timeInterval.ToChrono<std::chrono::nanoseconds>());
    }

    constexpr Point Point::Before(const Interval& timeInterval) const
    {
        return Point(value - timeInterval.ToChrono<std::chrono::nanoseconds>());
    }

    const Interval Point::ToInterval() const noexcept
    {
        return Interval(std::chrono::nanoseconds(value.time_since_epoch()));
    }

    constexpr Point& Point::operator+=(const Interval& d)
    {
        this->value += d.Value();
        return *this;
    }

    constexpr Point& Point::operator-=(const Interval& d)
    {
        this->value -= d.Value();
        return *this;
    }

#pragma mark - Literals
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wuser-defined-literals"

    constexpr Time::Interval operator""ns(unsigned long long ns)
    {
        return Time::Interval(std::chrono::nanoseconds(static_cast<std::chrono::nanoseconds::rep>(ns)));
    }

    constexpr Time::Interval operator""us(unsigned long long us)
    {
        return Time::Interval(std::chrono::microseconds(static_cast<std::chrono::microseconds::rep>(us)));
    }

    constexpr Time::Interval operator""ms(unsigned long long ms)
    {
        return Time::Interval(std::chrono::milliseconds(static_cast<std::chrono::milliseconds::rep>(ms)));
    }

    constexpr Time::Interval operator""s(unsigned long long s)
    {
        return Time::Interval(std::chrono::seconds(static_cast<std::chrono::seconds::rep>(s)));
    }

    constexpr Time::Interval operator""min(unsigned long long m)
    {
        return Time::Interval(std::chrono::minutes(static_cast<std::chrono::minutes::rep>(m)));
    }

    constexpr Time::Interval operator""h(unsigned long long h)
    {
        return Time::Interval(std::chrono::hours(static_cast<std::chrono::hours::rep>(h)));
    }

#if _LIBCPP_STD_VER > 17 && !defined(_LIBCPP_HAS_NO_CXX20_CHRONO_LITERALS)
    constexpr Time::Interval operator""d(unsigned long long d) noexcept
    {
        return Time::Interval(std::chrono::day(static_cast<std::chrono::day::rep>(d)));
    }

    constexpr Time::Interval operator""y(unsigned long long y) noexcept
    {
        return Time::Interval(std::chrono::year(static_cast<std::chrono::year::rep>(y)));
    }
#endif

#pragma clang diagnostic pop

    using nanoseconds = std::chrono::nanoseconds; // 纳秒
    using microseconds = std::chrono::microseconds; // 微秒
    using milliseconds = std::chrono::milliseconds; // 毫秒
    using seconds = std::chrono::seconds;
    using minutes = std::chrono::minutes;
    using hours = std::chrono::hours;

    using ns = nanoseconds; // 纳秒
    using us = microseconds; // 微秒
    using ms = milliseconds; // 毫秒
    using s = seconds;
    using min = minutes;

#if _LIBCPP_STD_VER > 17
    using day = std::chrono::day;
    using year = std::chrono::year;
#endif
}

}
