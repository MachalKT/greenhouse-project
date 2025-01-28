#pragma once

namespace common {
namespace utils {

template <typename TReturn, typename T>
constexpr TReturn msToUs(T numberMs) {
    return static_cast<TReturn>(numberMs) * static_cast<TReturn>(1000);
}

template <typename TReturn, typename T>
constexpr TReturn sToMs (T numberS) {
    return static_cast<TReturn>(numberS) * static_cast<TReturn>(1000);
}

} //utils
} //common
