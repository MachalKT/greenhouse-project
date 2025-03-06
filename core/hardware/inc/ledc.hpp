#pragma once

#include "igpio.hpp"
#include "types.hpp"

namespace hw {
class Ledc {
  public:
    enum class SpeedMode : uint8_t { HIGH, LOW };

    enum class TimerNumber : uint8_t { _0, _1, _2, _3 };

    enum class ClockSource : uint8_t { AUTO, APB, RC_FAST, REF_TICK };

    enum class Channel : uint8_t { _0, _1, _2, _3, _4, _5, _6, _7, _8 };

    struct Config {
        TimerNumber timerNumber;
        SpeedMode speedMode;
    };

    struct TimerConfig {
        uint32_t frequencyHz;
        ClockSource clockSource;
        uint8_t dutyResolution; // 1 - 20
    };

    struct ChannelConfig {
        Channel channel;
        uint32_t duty;
    };

    Ledc(Config config);

    common::Error init(TimerConfig& timerConfig);

    common::Error attachChannel(Channel channel, uint32_t duty, IGpio& gpio);

    common::Error setDuty(Channel channel, uint32_t duty);

  private:
    bool isDutyResolutionCorrect_(uint8_t& dutyResolution);

    uint8_t getClockSource_(ClockSource& clockSource);

    common::Error setupTimer_(TimerConfig& timerConfig);

    Config config_;
};
} // namespace hw
