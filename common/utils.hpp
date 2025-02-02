#pragma once

namespace common {
namespace utils {

/**
 * @brief Convert milliseconds to microseconds.
 */
template <typename TReturn, typename T> constexpr TReturn msToUs(T numberMs) {
  return static_cast<TReturn>(numberMs) * static_cast<TReturn>(1000);
}

/**
 * @brief Convert seconds to milliseconds.
 */
template <typename TReturn, typename T> constexpr TReturn sToMs(T numberS) {
  return static_cast<TReturn>(numberS) * static_cast<TReturn>(1000);
}

} // namespace utils
} // namespace common
