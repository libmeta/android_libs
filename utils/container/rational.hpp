#pragma once

namespace xlab {

template <typename T>
struct Rational {
    T num;
    T den;

    Rational() noexcept
    {
        num = den = static_cast<T>(0);
    }
};

}
