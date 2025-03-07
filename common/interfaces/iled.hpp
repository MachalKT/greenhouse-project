#pragma once

#include "types.hpp"
#include <cstdint>

namespace led {
enum class Color {
  RED,
  GREEN,
  BLUE,
  YELLOW,
  WHITE,
  ORANGE,
  CYAN,
  MAGENTA,
  LIME,
  PINK
};

class ILed {
    virtual common::Error setColor(const Color color) = 0;

    virtual common::Error setColor(const Color color, uint16_t brightness) = 0;
};
} // namespace led
