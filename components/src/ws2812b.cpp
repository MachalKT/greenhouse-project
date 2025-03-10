#include "ws2812b.hpp"

namespace ui {
Ws2812b::Ws2812b(Config config)
    : RgbBase(DUTY_RESOLUTION), config_{config},
      ledc{{hw::Ledc::TimerNumber::_0, hw::Ledc::SpeedMode::HIGH}} {}

common::Error Ws2812b::init() {
  hw::Ledc::TimerConfig timerConfig{};
  timerConfig.frequencyHz = PWM_FREQUENCY_HZ;
  timerConfig.dutyResolution = DUTY_RESOLUTION;
  timerConfig.clockSource = hw::Ledc::ClockSource::AUTO;

  common::Error errorCode = ledc.init(timerConfig);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode =
      ledc.attachChannel(hw::Ledc::Channel::_0, CHANNEL_DUTY, config_.redPin);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode =
      ledc.attachChannel(hw::Ledc::Channel::_1, CHANNEL_DUTY, config_.greenPin);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode =
      ledc.attachChannel(hw::Ledc::Channel::_2, CHANNEL_DUTY, config_.bluePin);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  return common::Error::OK;
}

common::Error Ws2812b::setColor(LedColor color) {
  RgbBase::setColor(color, getMaxBrightness_());

  ledc.setDuty(hw::Ledc::Channel::_0, rgbColor.red);
  ledc.setDuty(hw::Ledc::Channel::_1, rgbColor.green);
  ledc.setDuty(hw::Ledc::Channel::_2, rgbColor.blue);

  return common::Error::OK;
}

common::Error Ws2812b::setColor(const LedColor color, uint16_t brightness) {
  RgbBase::setColor(color, brightness);

  return common::Error::OK;
}

} // namespace ui
