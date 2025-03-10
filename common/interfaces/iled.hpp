#pragma once

#include "types.hpp"
#include <cstdint>

namespace ui {
enum class LedColor {
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
  public:
    virtual common::Error setColor(const LedColor color) = 0;

    virtual common::Error setColor(const LedColor color,
                                   uint16_t brightness) = 0;
};
} // namespace ui
