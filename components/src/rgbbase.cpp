#include "rgbbase.hpp"

namespace ui {
RgbBase::RgbBase(const uint8_t resolution) : resolution_{resolution} {}

common::Error RgbBase::setColor(const LedColor color, uint16_t brightness) {
  const uint16_t maxBrightness = getMaxBrightness_();
  brightness = brightness > maxBrightness ? maxBrightness : brightness;

  switch (color) {
  case LedColor::RED:
    rgbColor.red = brightness;
    rgbColor.green = BRIGHTNESS_OFF;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case LedColor::GREEN:
    rgbColor.red = BRIGHTNESS_OFF;
    rgbColor.green = brightness;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case LedColor::BLUE:
    rgbColor.red = BRIGHTNESS_OFF;
    rgbColor.green = BRIGHTNESS_OFF;
    rgbColor.blue = brightness;
    break;
  case LedColor::YELLOW:
    rgbColor.red = brightness;
    rgbColor.green = brightness;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case LedColor::WHITE:
    rgbColor.red = brightness;
    rgbColor.green = brightness;
    rgbColor.blue = brightness;
    break;
  case LedColor::ORANGE:
    rgbColor.red = brightness;
    rgbColor.green = brightness / 2;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case LedColor::CYAN:
    rgbColor.red = BRIGHTNESS_OFF;
    rgbColor.green = brightness;
    rgbColor.blue = brightness;
    break;
  case LedColor::MAGENTA:
    rgbColor.red = brightness;
    rgbColor.green = BRIGHTNESS_OFF;
    rgbColor.blue = brightness;
    break;
  case LedColor::LIME:
    rgbColor.red = brightness / 2;
    rgbColor.green = brightness;
    rgbColor.blue = BRIGHTNESS_OFF;
    break;
  case LedColor::PINK:
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

} // namespace ui
