#pragma once

#include "igpio.hpp"
#include "types.hpp"

namespace hw {
/**
 * @class Ledc
 * @brief Class representing an hardware LED controller.
 */
class Ledc {
  public:
    /**
     * @brief Enum representing different speed modes.
     */
    enum class SpeedMode : uint8_t {
      HIGH, ///< High speed mode
      LOW   ///< Low speed mode
    };

    /**
     * @brief Enum representing different timer numbers.
     */
    enum class TimerNumber : uint8_t {
      _0, ///< Timer 0
      _1, ///< Timer 1
      _2, ///< Timer 2
      _3  ///< Timer 3
    };

    /**
     * @brief Enum representing different clock sources.
     */
    enum class ClockSource : uint8_t {
      AUTO,    // Automatic clock source selection
      APB,     // APB clock source
      RC_FAST, // RC_FAST clock source
      REF_TICK // REF_TICK clock source
    };

    /**
     * @brief Enum representing different channels.
     */
    enum class Channel : uint8_t { _0, _1, _2, _3, _4, _5, _6, _7, _8 };

    /**
     * @brief Configuration structure for hardware LED controller.
     */
    struct Config {
        TimerNumber timerNumber;
        SpeedMode speedMode;
    };

    /**
     * @brief Configuration structure for timer.
     */
    struct TimerConfig {
        uint32_t frequencyHz;
        ClockSource clockSource;
        uint8_t dutyResolution; // Duty resolution (1 - 20)
    };

    /**
     * @brief Configuration structure for channel.
     */
    struct ChannelConfig {
        Channel channel;
        uint32_t duty;
    };

    /**
     * @brief Construct a new Ledc object.
     *
     * @param config Configuration for the hardware LED controller.
     */
    Ledc(Config config);

    /**
     * @brief Initialize the hardware LED controller.
     *
     * @param timerConfig Configuration for the timer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_ARG: Invalid argument.
     */
    common::Error init(TimerConfig& timerConfig);

    /**
     * @brief Attach a channel to the LED controller.
     *
     * @param channel Channel to be attached.
     * @param duty Duty cycle.
     * @param gpio GPIO pin for the channel.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_ARG: Invalid argument.
     */
    common::Error attachChannel(Channel channel, uint32_t duty, IGpio& gpio);

    /**
     * @brief Set the duty cycle for a channel.
     *
     * @param channel Channel to set the duty cycle for.
     * @param duty Duty cycle.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setDuty(Channel channel, uint32_t duty);

  private:
    /**
     * @brief Check if the duty resolution is correct.
     *
     * @param dutyResolution Duty resolution to check.
     *
     * @return
     *   - true: if the duty resolution is correct,
     *   - false otherwise.
     */
    bool isDutyResolutionCorrect_(uint8_t& dutyResolution);

    /**
     * @brief Get the clock source.
     *
     * @param clockSource Clock source to get.
     *
     * @return uint8_t representing the clock source.
     */
    uint8_t getClockSource_(ClockSource& clockSource);

    /**
     * @brief Setup the timer.
     *
     * @param timerConfig Configuration for the timer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setupTimer_(TimerConfig& timerConfig);

    Config config_;
};

} // namespace hw
