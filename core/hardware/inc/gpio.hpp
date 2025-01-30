#pragma once 
#include <cstdint>
#include <vector>

#include "types.hpp"

namespace hw {
class Gpio {
  public:
    enum class Pin : int8_t {
        PIN_NOT_ASSIGN = -2,
        PIN_0 = 0, PIN_1, PIN_2, PIN_3, PIN_4, PIN_5, PIN_6, PIN_7, PIN_8, PIN_9, PIN_10, 
        PIN_11, PIN_12, PIN_13, PIN_14, PIN_15, PIN_16, PIN_17, PIN_18, PIN_19, PIN_20, 
        PIN_21, PIN_22, PIN_23, PIN_24, PIN_25, PIN_26, PIN_27, PIN_28, PIN_29, 
        PIN_30, PIN_31, PIN32, PIN33,
        PIN_COUNT = 34
    };

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

    explicit Gpio(const Pin pin);

    common::Error setMode(const Mode mode);

    common::Error setLevel(const Level level);

    common::Error configurePullUpDown(const bool pullUpEnable, const bool pullDownEnable);

    Level getLevel() const;

    Pin getPin() const;

    common::Error setInterrupt(const InterruptType interruptType, common::Callback interruptCallback, common::CallbackData callbackData);

  private:
    common::Error setIsrService();

    static std::vector<Pin> usedPins_;
    bool isInterruptEnabled_{false};
    Pin pin_{Pin::PIN_NOT_ASSIGN};
    Mode mode_{Mode::DISABLE};
};


} //hw
