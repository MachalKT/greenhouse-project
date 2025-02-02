#include "gpio.hpp"

#include "driver/gpio.h"

namespace hw {

std::vector<common::PinNumber> Gpio::usedPinNumbers_{};

Gpio::Gpio(common::PinNumber pinNumber) {
  if (pinNumber < PIN_FIRST_NUMBER or pinNumber > PIN_LAST_NUMBER) {
    pinNumber_ = PIN_NOT_ASSIGN;
    return;
  }

  if (usedPinNumbers_.empty()) {
    pinNumber_ = pinNumber;
    usedPinNumbers_.push_back(pinNumber_);
    return;
  }

  for (auto& usedPin : usedPinNumbers_) {
    if (usedPin == pinNumber) {
      pinNumber_ = PIN_NOT_ASSIGN;
      return;
    }
  }

  pinNumber_ = pinNumber;
  usedPinNumbers_.push_back(pinNumber_);
}

common::Error Gpio::setMode(const Mode mode) {
  if (pinNumber_ == PIN_NOT_ASSIGN) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = gpio_set_direction(
      static_cast<gpio_num_t>(pinNumber_), static_cast<gpio_mode_t>(mode));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::setLevel(const Level level) {
  if (mode_ != Mode::OUTPUT) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = gpio_set_level(static_cast<gpio_num_t>(pinNumber_),
                                          static_cast<uint32_t>(level));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::configurePullUpDown(const bool pullUpEnable,
                                        const bool pullDownEnable) {
  if (pinNumber_ == PIN_NOT_ASSIGN) {
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
      gpio_set_pull_mode(static_cast<gpio_num_t>(pinNumber_), pullMode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

Gpio::Level Gpio::getLevel() const {
  if (pinNumber_ == PIN_NOT_ASSIGN) {
    return Level::LOW;
  }

  return static_cast<Level>(
      gpio_get_level(static_cast<gpio_num_t>(pinNumber_)));
}

common::PinNumber Gpio::getPin() const { return pinNumber_; }

common::Error Gpio::setInterrupt(const InterruptType interruptType,
                                 common::Callback interruptCallback,
                                 common::CallbackData callbackData) {
  if (pinNumber_ == PIN_NOT_ASSIGN) {
    return common::Error::INVALID_STATE;
  }

  if (not interruptCallback) {
    return common::Error::INVALID_ARG;
  }

  common::Error errorCode = setIsrService_();
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  esp_err_t espErrorCode =
      gpio_set_intr_type(static_cast<gpio_num_t>(pinNumber_),
                         static_cast<gpio_int_type_t>(interruptType));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = gpio_isr_handler_add(static_cast<gpio_num_t>(pinNumber_),
                                      interruptCallback, callbackData);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

bool Gpio::isPinAssigned() const { return pinNumber_ != PIN_NOT_ASSIGN; }

common::Error Gpio::setIsrService_() {
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
