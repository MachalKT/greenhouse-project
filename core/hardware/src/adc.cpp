#include "adc.hpp"

#include <algorithm>
#include <cmath>

#include "esp_adc/adc_oneshot.h"

namespace hw {

const Adc::Config Adc::INVALID_CONFIG{Unit::UNKNOWN, Channel::UNKNOWN,
                                      adc::BitWidth::DEFAULT};
std::vector<Adc::Config> Adc::usedConfig_{};
Adc::AdcHandle Adc::adcUnit1Handle_{};
Adc::AdcHandle Adc::adcUnit2Handle_{};

Adc::Adc(IGpio& adcGpio) {
  setConfig_(adcGpio);
  if (config_ == INVALID_CONFIG) {
    return;
  }

  if (usedConfig_.empty()) {
    usedConfig_.push_back(config_);
    return;
  }

  for (auto& usedConfig : usedConfig_) {
    if (usedConfig == config_) {
      config_ = INVALID_CONFIG;
      return;
    }
  }

  usedConfig_.push_back(config_);
}

common::Error Adc::init(const adc::BitWidth bitWidth,
                        const adc::Attenuation attenuation) {
  if (config_ == INVALID_CONFIG) {
    return common::Error::INVALID_STATE;
  }

  if (config_.unit == Unit::UNIT_1) {
    setAdcHandles_(adcHandle_, adcUnit1Handle_, ULP_MODE_ADC_UNIT_1);
  } else {
    setAdcHandles_(adcHandle_, adcUnit2Handle_, ULP_MODE_ADC_UNIT_2);
  }

  if (adcHandle_ == nullptr) {
    return common::Error::FAIL;
  }

  return setChannelConfiguration_(bitWidth, attenuation);
}

float Adc::readVoltage() {
  int adcValue{0};
  esp_err_t espErrorCode =
      adc_oneshot_read(static_cast<adc_oneshot_unit_handle_t>(adcHandle_),
                       static_cast<adc_channel_t>(config_.channel), &adcValue);
  if (espErrorCode != ESP_OK) {
    return 0.f;
  }

  return (static_cast<float>(adcValue) * 3.3f) /
         std::pow(2.f, static_cast<float>(config_.bitWidth));
}

common::Error Adc::deinit() {
  const auto unitCopy = config_.unit;

  auto it = std::find(usedConfig_.begin(), usedConfig_.end(), config_);
  if (it == usedConfig_.end()) {
    return common::Error::INVALID_STATE;
  }

  usedConfig_.erase(it);
  config_ = INVALID_CONFIG;
  adcHandle_ = nullptr;

  if (hasUnit_(unitCopy)) {
    return common::Error::OK;
  }

  return clearAdcUnitHandle_(unitCopy);
}

void Adc::setConfig_(IGpio& adcGpio) {
  if (not adcGpio.isGpioAssigned()) {
    return;
  }

  setUnitAndChannel_(adcGpio.getNumber());
}

void Adc::setUnitAndChannel_(const gpio::Number gpioNumber) {
  switch (gpioNumber) {
  case 0:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_1;
    break;
  case 2:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_2;
    break;
  case 4:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_0;
    break;
  case 12:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_5;
    break;
  case 13:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_4;
    break;
  case 14:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_6;
    break;
  case 15:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_3;
    break;
  case 25:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_8;
    break;
  case 26:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_9;
    break;
  case 27:
    config_.unit = Unit::UNIT_2;
    config_.channel = Channel::CHANNEL_7;
    break;
  case 32:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_4;
    break;
  case 33:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_5;
    break;
  case 34:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_6;
    break;
  case 35:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_7;
    break;
  case 36:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_0;
    break;
  case 37:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_1;
    break;
  case 38:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_2;
    break;
  case 39:
    config_.unit = Unit::UNIT_1;
    config_.channel = Channel::CHANNEL_3;
    break;
  default:
    config_.unit = Unit::UNKNOWN;
    config_.channel = Channel::UNKNOWN;
  }
}

void Adc::setAdcHandles_(AdcHandle& adcHandle, AdcHandle& adcUnitHandle,
                         const UlpMode ulpMode) {
  if (adcUnitHandle != nullptr) {
    adcHandle = adcUnitHandle;
    return;
  }

  adc_oneshot_unit_init_cfg_t adcConfig{};
  adcConfig.unit_id = static_cast<adc_unit_t>(config_.unit);
  adcConfig.ulp_mode = static_cast<adc_ulp_mode_t>(ulpMode);
  esp_err_t espErrorCode = adc_oneshot_new_unit(
      &adcConfig, reinterpret_cast<adc_oneshot_unit_handle_t*>(&adcUnitHandle));
  if (espErrorCode != ESP_OK) {
    adcUnitHandle = nullptr;
    adcHandle = nullptr;
    return;
  }

  adcHandle = adcUnitHandle;
}

common::Error
Adc::setChannelConfiguration_(const adc::BitWidth bitWidth,
                              const adc::Attenuation attenuation) {
  config_.bitWidth = bitWidth;

  adc_oneshot_chan_cfg_t channelConfig{};
  channelConfig.bitwidth = static_cast<adc_bitwidth_t>(config_.bitWidth);
  channelConfig.atten = static_cast<adc_atten_t>(attenuation);
  esp_err_t espErrorCode = adc_oneshot_config_channel(
      static_cast<adc_oneshot_unit_handle_t>(adcHandle_),
      static_cast<adc_channel_t>(config_.channel), &channelConfig);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

bool Adc::hasUnit_(const Unit unit) const {
  return std::any_of(usedConfig_.begin(), usedConfig_.end(),
                     [&unit](const Config& c) { return c.unit == unit; });
}

common::Error Adc::clearAdcUnitHandle_(const Unit unit) {
  esp_err_t espErrorCode{ESP_OK};
  if (unit == Unit::UNIT_1) {
    espErrorCode = adc_oneshot_del_unit(
        static_cast<adc_oneshot_unit_handle_t>(adcUnit1Handle_));
    adcUnit1Handle_ = nullptr;
  } else {
    espErrorCode = adc_oneshot_del_unit(
        static_cast<adc_oneshot_unit_handle_t>(adcUnit1Handle_));
    adcUnit2Handle_ = nullptr;
  }

  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

} // namespace hw
