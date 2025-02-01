#include "gpio.hpp"

#include "driver/gpio.h"

namespace hw {

std::vector<Gpio::Pin> Gpio::usedPins_{};

Gpio::Gpio(Pin pin) {
  if (usedPins_.empty()) {
    pin_ = pin;
    usedPins_.push_back(pin_);
    return;
  }

  for (auto& usedPin : usedPins_) {
    if (usedPin == pin) {
      pin_ = Pin::PIN_NOT_ASSIGN;
      return;
    }
  }

  pin_ = pin;
  usedPins_.push_back(pin_);
}

common::Error Gpio::setMode(const Mode mode) {
  if (pin_ == Pin::PIN_NOT_ASSIGN) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = gpio_set_direction(static_cast<gpio_num_t>(pin_),
                                              static_cast<gpio_mode_t>(mode));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::setLevel(const Level level) {
  if (mode_ != Mode::OUTPUT) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = gpio_set_level(static_cast<gpio_num_t>(pin_),
                                          static_cast<uint32_t>(level));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::configurePullUpDown(const bool pullUpEnable,
                                        const bool pullDownEnable) {
  if (pin_ == Pin::PIN_NOT_ASSIGN) {
    return common::Error::INVALID_STATE;
  }

  gpio_pull_mode_t pullMode{GPIO_FLOATING};
  if (pullUpEnable and pullDownEnable) {
    pullMode = GPIO_PULLUP_PULLDOWN;
  } else if (pullUpEnable) {
    pullMode = GPIO_PULLUP_ONLY;
  } else if (pullDownEnable) {
    pullMode = GPIO_PULLDOWN_ONLY;
  }

  esp_err_t espErrorCode =
      gpio_set_pull_mode(static_cast<gpio_num_t>(pin_), pullMode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

Gpio::Level Gpio::getLevel() const {
  if (pin_ == Pin::PIN_NOT_ASSIGN) {
    return Level::LOW;
  }

  return static_cast<Level>(gpio_get_level(static_cast<gpio_num_t>(pin_)));
}

Gpio::Pin Gpio::getPin() const { return pin_; }

common::Error Gpio::setInterrupt(const InterruptType interruptType,
                                 common::Callback interruptCallback,
                                 common::CallbackData callbackData) {
  if (pin_ == Pin::PIN_NOT_ASSIGN) {
    return common::Error::INVALID_STATE;
  }

  if (not interruptCallback) {
    return common::Error::INVALID_ARG;
  }

  common::Error errorCode = setIsrService();
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  esp_err_t espErrorCode =
      gpio_set_intr_type(static_cast<gpio_num_t>(pin_),
                         static_cast<gpio_int_type_t>(interruptType));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = gpio_isr_handler_add(static_cast<gpio_num_t>(pin_),
                                      interruptCallback, callbackData);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::setIsrService() {
  if (isInterruptEnabled_) {
    return common::Error::OK;
  }

  esp_err_t espErrorCode = gpio_install_isr_service(0);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  isInterruptEnabled_ = true;
  return common::Error::OK;
}

} // namespace hw
