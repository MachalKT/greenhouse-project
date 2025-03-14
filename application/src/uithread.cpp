#include "uithread.hpp"
#include "delay.hpp"
#include "esp_log.h"
#include <string_view>

namespace {
std::string_view TAG{"UI"};
} // namespace

namespace app {
UiThread::UiThread(Config config)
    : ThreadBase{{"RadioThread", STACK_DEPTH, PRIORITY, CORE_ID}},
      config_{config} {}

void UiThread::run_() {
  config_.ledTimer.setCallback(
      [](common::Argument arg) {
        assert(arg);
        UiThread* thread = static_cast<UiThread*>(arg);
        thread->handleLedEvent_(thread->radioLedEvent_);
      },
      this);

  config_.button.setCallback(
      [](common::Argument arg) { ESP_LOGI("BUTTON", "Click"); }, this);

  config_.queue.setCallback(
      [](def::ui::LedEvent event, common::Argument arg) {
        assert(arg);
        UiThread* thread = static_cast<UiThread*>(arg);
        thread->handleLedEvent_(event);
      },
      this);

  while (1) {
    config_.button.yield();
    config_.queue.yield();
    sw::delayMs(10);
  }
}

void UiThread::handleLedEvent_(def::ui::LedEvent& event) {
  ui::LedColor color{ui::LedColor::BLUE};

  using def::ui::LedEvent;
  switch (event) {
  case LedEvent::WIFI_CONNECTION:
    wifiLedEvent_ = event;
    color = ui::LedColor::BLUE;
    break;
  case LedEvent::WIFI_CONNECTED:
    wifiLedEvent_ = event;
    color = ui::LedColor::GREEN;
    break;
  case LedEvent::WIFI_DISCONNECTED:
    wifiLedEvent_ = event;
    color = ui::LedColor::RED;
    setLedTimer_();
    break;
  case LedEvent::RADIO_COMMUNICATION:
    radioLedEvent_ = event;
    break;
  case LedEvent::RADIO_TIMEOUT:
    radioLedEvent_ = event;
    color = ui::LedColor::MAGENTA;
    break;
  default:
    color = ui::LedColor::ORANGE;
  }

  common::Error errorCode = config_.led.setColor(color);
  if (errorCode != common::Error::OK) {
    ESP_LOGI(TAG.data(), "set color fail");
  }
}

void UiThread::setLedTimer_() {
  if (radioLedEvent_ == def::ui::LedEvent::RADIO_TIMEOUT) {
    config_.ledTimer.startOnce(DISCONNECTED_LED_TIME_US);
  }
}

} // namespace app
