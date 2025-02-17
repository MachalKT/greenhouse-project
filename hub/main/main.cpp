#include "delay.hpp"
#include "esp_log.h"
#include "hrtimer.hpp"
#include "nvsstore.hpp"
#include "wificontroller.hpp"
#include <string_view>

namespace {
static constexpr std::string_view TAG{"HUB"};
}

extern "C" {
void app_main(void) {
  common::Error errorCode{common::Error::OK};

  errorCode = storage::hw::NvsStore::init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "NVS init fail");
  }

  timer::hw::HrTimer wifiReconnectTimer;
  errorCode = wifiReconnectTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "wifiReconnectTimer init fail");
  }

  app::WifiController wifiController{wifiReconnectTimer};
  errorCode = wifiController.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "wifiController init fail");
  }

  errorCode = wifiController.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "WifiController start fail");
  }

  while (1) {
    sw::delayMs(10);
  }
}
}
