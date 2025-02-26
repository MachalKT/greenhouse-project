#pragma once

#include "types.hpp"

namespace hw {
using GpioNumber = int8_t;
static constexpr int8_t INVALID_GPIO_NUMBER{-1};

enum class GpioMode : uint8_t {
  DISABLE = 0,
  INPUT,
  OUTPUT,
};

enum class GpioLevel : uint8_t {
  LOW = 0,
  HIGH,
};

enum class GpioInterruptType : uint8_t {
  DISABLE = 0,
  RISING_EDGE,
  FALLING_EDGE,
  RISING_AND_FALLING_EDGE,
  LOW_LEVEL,
  HIGH_LEVEL,
};

class IGpio {
  public:
    virtual common::Error setMode(const GpioMode mode) = 0;

    virtual common::Error setLevel(const GpioLevel level) = 0;

    virtual common::Error configurePullUpDown(const bool pullUpEnable,
                                              const bool pullDownEnable) = 0;

    virtual GpioLevel getLevel() const = 0;

    virtual GpioNumber getNumber() const = 0;

    virtual common::Error setInterrupt(const GpioInterruptType interruptType,
                                       common::Callback interruptCallback,
                                       common::Argument callbackData) = 0;

    virtual bool isGpioAssigned() const = 0;
};
} // namespace hw
