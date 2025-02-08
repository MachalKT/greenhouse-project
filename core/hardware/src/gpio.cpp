#include "gpio.hpp"

#include "driver/gpio.h"

namespace hw {

std::vector<gpio::Number> Gpio::usedGpioNumbers_{};

Gpio::Gpio(gpio::Number number) {
  if (number < FIRST_NUMBER or number > LAST_NUMBER) {
    number_ = gpio::INVALID_NUMBER;
    return;
  }

  if (usedGpioNumbers_.empty()) {
    number_ = number;
    usedGpioNumbers_.push_back(number_);
    return;
  }

  for (auto& usedNumber : usedGpioNumbers_) {
    if (usedNumber == number) {
      number_ = gpio::INVALID_NUMBER;
      return;
    }
  }

  number_ = number;
  usedGpioNumbers_.push_back(number_);
}

common::Error Gpio::setMode(const gpio::Mode mode) {
  if (number_ == gpio::INVALID_NUMBER) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = gpio_set_direction(static_cast<gpio_num_t>(number_),
                                              static_cast<gpio_mode_t>(mode));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::setLevel(const gpio::Level level) {
  if (mode_ != gpio::Mode::OUTPUT) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = gpio_set_level(static_cast<gpio_num_t>(number_),
                                          static_cast<uint32_t>(level));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Gpio::configurePullUpDown(const bool pullUpEnable,
                                        const bool pullDownEnable) {
  if (number_ == gpio::INVALID_NUMBER) {
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
      gpio_set_pull_mode(static_cast<gpio_num_t>(number_), pullMode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

gpio::Level Gpio::getLevel() const {
  if (number_ == gpio::INVALID_NUMBER) {
    return gpio::Level::LOW;
  }

  return static_cast<gpio::Level>(
      gpio_get_level(static_cast<gpio_num_t>(number_)));
}

gpio::Number Gpio::getNumber() const { return number_; }

common::Error Gpio::setInterrupt(const gpio::InterruptType interruptType,
                                 common::Callback interruptCallback,
                                 common::CallbackData callbackData) {
  if (number_ == gpio::INVALID_NUMBER) {
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
      gpio_set_intr_type(static_cast<gpio_num_t>(number_),
                         static_cast<gpio_int_type_t>(interruptType));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = gpio_isr_handler_add(static_cast<gpio_num_t>(number_),
                                      interruptCallback, callbackData);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

bool Gpio::isGpioAssigned() const { return number_ != gpio::INVALID_NUMBER; }

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
