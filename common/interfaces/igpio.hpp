#pragma once

#include "types.hpp"

namespace hw {
namespace gpio {
using Number = int8_t;
enum class Mode : uint8_t;
enum class Level : uint8_t;
enum class InterruptType : uint8_t;
static constexpr int8_t INVALID_NUMBER{-1};
} // namespace gpio

class IGpio {
  public:
    virtual common::Error setMode(const gpio::Mode mode) = 0;

    virtual common::Error setLevel(const gpio::Level level) = 0;

    virtual common::Error configurePullUpDown(const bool pullUpEnable,
                                              const bool pullDownEnable) = 0;

    virtual gpio::Level getLevel() const = 0;

    virtual gpio::Number getNumber() const = 0;

    virtual common::Error setInterrupt(const gpio::InterruptType interruptType,
                                       common::Callback interruptCallback,
                                       common::Argument callbackData) = 0;

    virtual bool isGpioAssigned() const = 0;
};
} // namespace hw
