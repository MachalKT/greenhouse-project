#pragma once

#include "igpio.hpp"
#include "iled.hpp"
#include "ledc.hpp"
#include "rgbbase.hpp"
#include "types.hpp"

namespace led {

/**
 * @class Ws2812b
 * @brief Class representing the WS2812B RGB LED.
 */
class Ws2812b final : public RgbBase {
  public:
    /**
     * @brief Configuration structure for WS2812B.
     */
    struct Config {
        hw::IGpio& redPin;
        hw::IGpio& greenPin;
        hw::IGpio& bluePin;
    };

    /**
     * @brief Construct a new Ws2812b object.
     *
     * @param config Configuration for the WS2812B.
     */
    Ws2812b(Config config);

    /**
     * @brief Initialize the WS2812B LED.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init();

    /**
     * @brief Set the color of the WS2812B LED.
     *
     * @param color Color to set.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setColor(Color color) override;

    /**
     * @brief Set the color and brightness of the WS2812B LED.
     *
     * @param color Color to set.
     * @param brightness Brightness level.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setColor(const Color color, uint16_t brightness) override;

  private:
    static constexpr uint32_t PWM_FREQUENCY_HZ{1000};
    static constexpr uint32_t CHANNEL_DUTY{0};
    static constexpr uint8_t DUTY_RESOLUTION{8};

    Config config_;
    hw::Ledc ledc;
};

} // namespace led
