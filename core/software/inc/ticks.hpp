#pragma once

#include "freertos/FreeRTOS.h"
#include "types.hpp"
#include <cstdint>

namespace sw {
using Ticks = uint32_t;

inline Ticks msToTicks(const common::Time timeMs) {
  return timeMs / portTICK_PERIOD_MS;
}

inline Ticks usToTicks(const common::Time timeUs) {
  constexpr common::Time US_PER_MS{1000};
  return msToTicks(timeUs / US_PER_MS);
}
} // namespace sw
