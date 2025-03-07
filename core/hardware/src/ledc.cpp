#include "ledc.hpp"
#include "driver/ledc.h"

namespace hw {
Ledc::Ledc(Config config) : config_{config} {}

common::Error Ledc::init(TimerConfig& timerConfig) {
  if (not isDutyResolutionCorrect_(timerConfig.dutyResolution)) {
    return common::Error::INVALID_ARG;
  }

  common::Error errorCode = setupTimer_(timerConfig);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  return common::Error::OK;
}

common::Error Ledc::attachChannel(Channel channel, uint32_t duty, IGpio& gpio) {
  if (not gpio.isGpioAssigned()) {
    return common::Error::INVALID_ARG;
  }

  ledc_channel_config_t channelConfig{};
  channelConfig.channel = static_cast<ledc_channel_t>(channel);
  channelConfig.gpio_num = static_cast<int>(gpio.getNumber());
  channelConfig.speed_mode = static_cast<ledc_mode_t>(config_.speedMode);
  channelConfig.timer_sel = static_cast<ledc_timer_t>(config_.timerNumber);
  channelConfig.duty = duty;

  esp_err_t espErrorCode = ledc_channel_config(&channelConfig);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Ledc::setDuty(Channel channel, uint32_t duty) {
  esp_err_t espErrorCode =
      ledc_set_duty(static_cast<ledc_mode_t>(config_.speedMode),
                    static_cast<ledc_channel_t>(channel), duty);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = ledc_update_duty(static_cast<ledc_mode_t>(config_.speedMode),
                                  static_cast<ledc_channel_t>(channel));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

bool Ledc::isDutyResolutionCorrect_(uint8_t& dutyResolution) {
  constexpr uint8_t MIN_DUTY_RESOLUTION{1};
  constexpr uint8_t MAX_DUTY_RESOLUTION{20};

  if (dutyResolution < MIN_DUTY_RESOLUTION ||
      dutyResolution > MAX_DUTY_RESOLUTION) {
    return false;
  }

  return true;
}

uint8_t Ledc::getClockSource_(ClockSource& clockSource) {
  if (clockSource == ClockSource::AUTO) {
    return static_cast<uint8_t>(LEDC_AUTO_CLK);
  } else if (clockSource == ClockSource::APB) {
    return static_cast<uint8_t>(LEDC_USE_APB_CLK);
  } else if (clockSource == ClockSource::RC_FAST) {
    return static_cast<uint8_t>(LEDC_USE_RC_FAST_CLK);
  }

  return static_cast<uint8_t>(LEDC_USE_REF_TICK);
}

common::Error Ledc::setupTimer_(TimerConfig& timerConfig) {
  ledc_timer_config_t ledcTimerConfig{};
  ledcTimerConfig.speed_mode = static_cast<ledc_mode_t>(config_.speedMode);
  ledcTimerConfig.timer_num = static_cast<ledc_timer_t>(config_.timerNumber);
  ledcTimerConfig.duty_resolution =
      static_cast<ledc_timer_bit_t>(timerConfig.dutyResolution);
  ledcTimerConfig.freq_hz = timerConfig.frequencyHz;
  ledcTimerConfig.clk_cfg = static_cast<ledc_clk_cfg_t>(
      getClockSource_(timerConfig.clockSource)); // LEDC_AUTO_CLK;

  esp_err_t espErrorCode = ledc_timer_config(&ledcTimerConfig);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

} // namespace hw
