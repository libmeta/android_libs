
#ifndef XLIVE_GLOBAL_FUNCTION_HPP
#define XLIVE_GLOBAL_FUNCTION_HPP

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
#define _IS_CAN_DEMANGLE_
#endif

namespace xlab::GlobalFunc {

#ifdef _IS_CAN_DEMANGLE_

static inline std::string demangle(const char* name)
{
    int status = -4;
    std::unique_ptr<char, void (*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };
    return std::string(status == 0 ? res.get() : name);
}

#else

static inline std::string demangle(const char* name)
{
    return std::string(name);
}

#endif

template <typename T>
static inline T Clamp(T val, T min, T max)
{
    if (val < min) {
        return min;
    } else if (val > max) {
        return max;
    }
    return val;
}

template <typename T>
static inline std::vector<uint8_t> num2BytVct(T val)
{
    std::vector<uint8_t> tmp(sizeof(T));
    memcpy(tmp.data(), &val, tmp.size());
    return tmp;
}

static inline std::vector<uint8_t> num2BytVct(uint32_t val)
{
    std::vector<uint8_t> tmp(sizeof(uint32_t));
    memcpy(tmp.data(), &val, tmp.size());
    return tmp;
}

template <typename T>
static inline T bytVct2Num(std::vector<uint8_t> val)
{
    val.resize(sizeof(T));
    return *reinterpret_cast<T*>(val.data());
}

static inline uint32_t bytVct2Num(std::vector<uint8_t> val)
{
    val.resize(sizeof(uint32_t));
    return *reinterpret_cast<uint32_t*>(val.data());
}

template <typename T>
static inline std::vector<T> reverseVct(std::vector<T> val)
{
    std::reverse(val.begin(), val.end());
    return val;
}

static inline std::vector<uint8_t> reverseVct(std::vector<uint8_t> val)
{
    std::reverse(val.begin(), val.end());
    return val;
}

template <typename T>
static inline T reverseNum(T val)
{
    return bytVct2Num<T>(reverseVct(num2BytVct<T>(val)));
}

static inline uint32_t reverseNum(uint32_t val)
{
    return bytVct2Num(reverseVct(num2BytVct(val)));
}

}

#endif // XLIVE_GLOBAL_FUNCTION_HPP
