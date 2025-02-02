#pragma once
#include <cstdint>
#include <vector>

#include "types.hpp"

namespace hw {
class Gpio {
  public:
    enum class Mode : uint8_t {
      DISABLE = 0,
      INPUT,
      OUTPUT,
    };

    enum class Level : uint8_t {
      LOW = 0,
      HIGH,
    };

    enum class InterruptType : uint8_t {
      DISABLE = 0,
      RISING_EDGE,
      FALLING_EDGE,
      RISING_AND_FALLING_EDGE,
      LOW_LEVEL,
      HIGH_LEVEL,
    };

    explicit Gpio(const common::PinNumber pinNumber);

    common::Error setMode(const Mode mode);

    common::Error setLevel(const Level level);

    common::Error configurePullUpDown(const bool pullUpEnable,
                                      const bool pullDownEnable);

    Level getLevel() const;

    common::PinNumber getPin() const;

    common::Error setInterrupt(const InterruptType interruptType,
                               common::Callback interruptCallback,
                               common::CallbackData callbackData);

    bool isPinAssigned() const;

  private:
    common::Error setIsrService();

    static constexpr int8_t PIN_NOT_ASSIGN{-1};
    static constexpr int8_t PIN_FIRST_NUMBER{0};
    static constexpr int8_t PIN_LAST_NUMBER{33};
    static std::vector<common::PinNumber> usedPinNumbers_;
    bool isInterruptEnabled_{false};
    int8_t pinNumber_{PIN_NOT_ASSIGN};
    Mode mode_{Mode::DISABLE};
};

} // namespace hw
