#pragma once

#include "types.hpp"

namespace hw {
namespace gpio {
using PinNumber = int8_t;
enum class Mode : uint8_t;
enum class Level : uint8_t;
enum class InterruptType : uint8_t;
} // namespace gpio

class IGpio {
  public:
    virtual common::Error setMode(const gpio::Mode mode) = 0;

    virtual common::Error setLevel(const gpio::Level level) = 0;

    virtual common::Error configurePullUpDown(const bool pullUpEnable,
                                              const bool pullDownEnable) = 0;

    virtual gpio::Level getLevel() const = 0;

    virtual gpio::PinNumber getPin() const = 0;

    virtual common::Error setInterrupt(const gpio::InterruptType interruptType,
                                       common::Callback interruptCallback,
                                       common::CallbackData callbackData) = 0;

    virtual bool isPinAssigned() const = 0;
};
} // namespace hw
