#include "rgbbase.hpp"

namespace led {
RgbBase::RgbBase(const uint8_t resolution) : resolution_{resolution} {}

common::Error RgbBase::setColor(const Color color, uint16_t brightness) {
  const uint16_t maxBrightness = getMaxBrightness_();
  brightness = brightness > maxBrightness ? maxBrightness : brightness;

  switch (color) {
  case Color::RED:
    rgbColor.red = brightness;
    rgbColor.green = BRIGHTNESS_OFF;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case Color::GREEN:
    rgbColor.red = BRIGHTNESS_OFF;
    rgbColor.green = brightness;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case Color::BLUE:
    rgbColor.red = BRIGHTNESS_OFF;
    rgbColor.green = BRIGHTNESS_OFF;
    rgbColor.blue = brightness;
    break;
  case Color::YELLOW:
    rgbColor.red = brightness;
    rgbColor.green = brightness;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case Color::WHITE:
    rgbColor.red = brightness;
    rgbColor.green = brightness;
    rgbColor.blue = brightness;
    break;
  case Color::ORANGE:
    rgbColor.red = brightness;
    rgbColor.green = brightness / 2;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case Color::CYAN:
    rgbColor.red = BRIGHTNESS_OFF;
    rgbColor.green = brightness;
    rgbColor.blue = brightness;
    break;
  case Color::MAGENTA:
    rgbColor.red = brightness;
    rgbColor.green = BRIGHTNESS_OFF;
    rgbColor.blue = brightness;
    break;
  case Color::LIME:
    rgbColor.red = brightness / 2;
    rgbColor.green = brightness;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case Color::PINK:
    rgbColor.red = brightness;
    rgbColor.green = brightness / 2;
    rgbColor.blue = brightness / 2;
    break;
  default:
    return common::Error::INVALID_ARG;
  }

  return common::Error::OK;
}

uint16_t RgbBase::getMaxBrightness_() const { return (1 << resolution_) - 1; }

} // namespace led
