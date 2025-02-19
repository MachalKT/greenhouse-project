#pragma once

#include "types.hpp"
#include <cstdint>

namespace sw {

/**
 * @brief Delay for a given time in milliseconds.
 *
 * @param timeMs Time in milliseconds.
 */
void delayMs(const common::Time timeMs);

} // namespace sw
