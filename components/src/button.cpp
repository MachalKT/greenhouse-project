#include "button.hpp"
#include "assert.h"
#include "delay.hpp"
#include "esp_log.h"

namespace ui {
Button::Button(hw::IGpio& buttonPin) : buttonPin_{buttonPin} {}

common::Error Button::init() {
  common::Error errorCode = buttonPin_.setMode(hw::GpioMode::INPUT);
  if (errorCode != common::Error::OK) {
    ESP_LOGE("BUTTON", "set mode fail");
    return errorCode;
  }

  errorCode = buttonPin_.configurePullUpDown(true, true);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode = buttonPin_.setInterrupt(
      hw::GpioInterruptType::FALLING_EDGE,
      [](void* arg) {
        assert(arg);
        Button* button = static_cast<Button*>(arg);
        button->buttonTriggered_ = true;
      },
      this);

  if (errorCode != common::Error::OK) {
    ESP_LOGE("BUTTON", "setInterrupt fail %d", static_cast<int>(errorCode));
    return errorCode;
  }

  return common::Error::OK;
}

void Button::setCallback(common::Callback cb, common::Argument arg) {
  cb_ = cb;
  arg_ = arg;
}

void Button::yield() {
  if (buttonTriggered_) {
    sw::delayMs(100);
    buttonTriggered_ = false;
    if (cb_) {
      cb_(arg_);
    }
  }
}

} // namespace ui
