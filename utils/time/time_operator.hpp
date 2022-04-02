#pragma once

#include "time.hpp"

namespace xlab {

namespace Time {

#pragma mark - Interval Operator

    static inline bool operator==(const Interval& i1, const Interval& i2)
    {
        return i1.Value() == i2.Value();
    }

    static inline bool operator!=(const Interval& i1, const Interval& i2)
    {
        return i1.Value() != i2.Value();
    }

    static inline bool operator>(const Interval& i1, const Interval& i2)
    {
        return i1.Value() > i2.Value();
    }

    static inline bool operator>=(const Interval& i1, const Interval& i2)
    {
        return i1.Value() >= i2.Value();
    }

    static inline bool operator<(const Interval& i1, const Interval& i2)
    {
        return i1.Value() < i2.Value();
    }

    static inline bool operator<=(const Interval& i1, const Interval& i2)
    {
        return i1.Value() <= i2.Value();
    }

    static inline Interval operator-(const Interval& i1, const Interval& i2)
    {
        return Interval(i1.Value() - i2.Value());
    }

    static inline Interval operator+(const Interval& i1, const Interval& i2)
    {
        return Interval(i1.Value() + i2.Value());
    }

    template <typename T>
    static inline Interval operator/(const Interval& i, const T& t)
    {
        return Interval(i.Value() / t);
    }

#pragma mark - Point Operator

    static inline bool operator==(const Point& p1, const Point& p2)
    {
        return p1.Value() == p2.Value();
    }

    static inline bool operator!=(const Point& p1, const Point& p2)
    {
        return p1.Value() != p2.Value();
    }

    static inline bool operator>(const Point& p1, const Point& p2)
    {
        return p1.Value() > p2.Value();
    }

    static inline bool operator>=(const Point& p1, const Point& p2)
    {
        return p1.Value() >= p2.Value();
    }

    static inline bool operator<(const Point& p1, const Point& p2)
    {
        return p1.Value() < p2.Value();
    }

    static inline bool operator<=(const Point& p1, const Point& p2)
    {
        return p1.Value() <= p2.Value();
    }

    static inline Point operator-(const Point& t, const Interval& d)
    {
        return Point(t.Value() - d.Value());
    }

    static inline Point operator+(const Point& t, const Interval& d)
    {
        return Point(t.Value() + d.Value());
    }

    static inline Interval operator-(const Point& p1, const Point& p2)
    {
        return Interval(p1.Value() - p2.Value());
    }

}

}
