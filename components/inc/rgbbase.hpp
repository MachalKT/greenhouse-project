#pragma once

#include "iled.hpp"
#include "types.hpp"

namespace ui {
/**
 * @class RgbBase
 * @brief Base class for RGB LED control.
 */
class RgbBase : public ILed {
  public:
    /**
     * @brief Construct a new RgbBase object.
     *
     * @param resolution Resolution for the RGB LED.
     */
    RgbBase(const uint8_t resolution);

    /**
     * @brief Set the color of the RGB LED.
     *
     * @param color Color to set.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    virtual common::Error setColor(const LedColor color) = 0;

    /**
     * @brief Set the color and brightness of the RGB LED.
     *
     * @param color Color to set.
     * @param brightness Brightness level.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setColor(const LedColor color, uint16_t brightness) override;

  protected:
    /**
     * @brief Structure representing RGB color values.
     */
    struct RgbColor {
        uint16_t red;
        uint16_t green;
        uint16_t blue;
    };

    /**
     * @brief Get the maximum brightness level.
     *
     * @return uint16_t Maximum brightness level.
     */
    uint16_t getMaxBrightness_() const;

    RgbColor rgbColor{0, 0, 0};

  private:
    static constexpr uint16_t BRIGHTNESS_OFF{0};

    const uint8_t resolution_;
};

} // namespace ui
