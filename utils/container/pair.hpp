#pragma once

#include <algorithm>

namespace xlab {

struct Pair {
    int first;
    int second;

    Pair()
        : first(0)
        , second(0)
    {
    }

    Pair(int f, int s)
        : first(f)
        , second(s)
    {
    }

    const Pair& Swap()
    {
        std::swap(first, second);
        return *this;
    }

    const Pair& Swap(int angle)
    {
        if (angle == 90 || angle == 270) {
            this->Swap();
        }
        return *this;
    }

    void AlignmentFirst(uint32_t align)
    {
        first = ((first + align - 1) / align) * align;
    }

    void AlignmentSecond(uint32_t align)
    {
        second = ((second + align - 1) / align) * align;
    }

    void Bzero()
    {
        first = 0;
        second = 0;
    }

    bool IsZero()
    {
        return first == 0 || second == 0;
    }

    bool IsLessThanOREqualToZero()
    {
        return first <= 0 || second <= 0;
    }

    void SetValIfZero(const Pair& p)
    {
        if (this->IsZero()) {
            *this = p;
            // first = p.first;
            // second = p.second;
        }
    }

    Pair Alignment(uint32_t align)
    {
        first = ((first + align - 1) / align) * align;
        second = ((second + align - 1) / align) * align;
        return { first, second };
    }

    size_t Multiply()
    {
        return static_cast<size_t>(first) * static_cast<size_t>(second);
    }

    size_t AreaSize()
    {
        return first * second;
    }

    bool IndexOf(int val)
    {
        return val >= first && val <= second;
    }

    bool EqualItem(Pair val_pair)
    {
        return *this == val_pair || *this == val_pair.Swap();
    }

    Pair operator+(const Pair& p) const
    {
        return Pair(this->first + p.first, this->second + p.second);
    }

    template <typename T>
    Pair operator+(const T& t) const
    {
        return Pair(this->first + t, this->second + t);
    }

    template <typename T>
    Pair operator+(const std::pair<T, T>& t) const
    {
        return Pair(static_cast<int>(this->first + t.first), static_cast<int>(this->second + t.first));
    }

    Pair operator-(const Pair& p) const
    {
        return Pair(this->first - p.first, this->second - p.second);
    }

    template <typename T>
    Pair operator-(const T& t) const
    {
        return Pair(this->first - t, this->second - t);
    }

    template <typename T>
    Pair operator-(const std::pair<T, T>& t) const
    {
        return Pair(static_cast<int>(this->first - t.first), static_cast<int>(this->second - t.first));
    }

    Pair operator*(const Pair& p) const
    {
        return Pair(this->first * p.first, this->second * p.second);
    }

    template <typename T>
    Pair operator*(const T& t) const
    {
        return Pair(this->first * t, this->second * t);
    }

    template <typename T>
    Pair operator*(const std::pair<T, T>& t) const
    {
        return Pair(static_cast<int>(this->first * t.first), static_cast<int>(this->second * t.second));
    }

    Pair operator/(const Pair& p) const
    {
        return Pair(this->first / p.first, this->second / p.second);
    }

    template <typename T>
    Pair operator/(const T& t) const
    {
        return Pair(this->first / t, this->second / t);
    }

    template <typename T>
    Pair operator/(const std::pair<T, T>& t) const
    {
        return Pair(static_cast<int>(this->first / t.first), static_cast<int>(this->second / t.second));
    }

    Pair& operator*=(const Pair& p)
    {
        this->first *= p.first;
        this->second *= p.second;
        return *this;
    }

    template <typename T>
    Pair& operator*=(const T& t)
    {
        this->first *= t;
        this->second *= t;
        return *this;
    }

    template <typename T>
    Pair& operator*=(const std::pair<T, T>& t)
    {
        this->first *= t.first;
        this->second *= t.second;
        return *this;
    }

    Pair& operator/=(const Pair& p)
    {
        this->first /= p.first;
        this->second /= p.second;
        return *this;
    }

    template <typename T>
    Pair& operator/=(const T& t)
    {
        this->first /= t;
        this->second /= t;
        return *this;
    }

    template <typename T>
    Pair& operator/=(const std::pair<T, T>& t)
    {
        this->first /= t.first;
        this->second /= t.second;
        return *this;
    }

    bool operator==(const Pair& p) const
    {
        return first == p.first && second == p.second;
    }

    bool operator!=(const Pair& p) const
    {
        return !this->operator==(std::move(p));
    }

    bool operator>(const Pair& p) const
    {
        return this->first > p.first && this->second > p.second;
    }

    bool operator>=(const Pair& p) const
    {
        return this->first >= p.first && this->second >= p.second;
    }

    bool operator<(const Pair& p) const
    {
        return this->first < p.first && this->second < p.second;
    }

    bool operator<=(const Pair& p) const
    {
        return this->first <= p.first && this->second <= p.second;
    }
};

}

namespace std {

template <>
struct hash<xlab::Pair> {
    size_t operator()(const xlab::Pair& Pair) const
    {
        const auto h1 = std::hash<int>()(Pair.first);
        const auto h2 = std::hash<int>()(Pair.second);
        return h1 ^ (h2 << 1);
    }
};

}
