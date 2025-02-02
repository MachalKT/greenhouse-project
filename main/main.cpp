#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.hpp"
#include "hrtimer.hpp"
#include "i2c.hpp"
#include "sensorscontroller.hpp"
#include "sht40.hpp"
#include "spi.hpp"
#include "utils.hpp"

namespace {
const char* TAG = "MAIN";
static constexpr common::Time MEASUREMENT_TIME_US{
    common::utils::msToUs<common::Time, common::Time>(
        common::utils::sToMs<common::Time, common::Time>(1))};
} // namespace

extern "C" {
void app_main(void) {

  hw::Gpio miso{19};
  hw::Gpio mosi{23};
  hw::Gpio sck{18};
  hw::Spi spi{{miso, mosi, sck, hw::Spi::Host::VSPI}};

  common::Error error = spi.init(hw::Spi::DmaChannel::CHANNEL_1);
  if (error != common::Error::OK) {
    ESP_LOGE(TAG, "spi init fail");
  }

  hw::Gpio cs{5};
  hw::spi::DeviceHandle deviceHandle{nullptr};
  error = spi.addDevice(deviceHandle, cs.getPin(), hw::Spi::CLOCK_SPEED_HZ);
  if (error != common::Error::OK) {
    ESP_LOGE(TAG, "spi add device fail");
  }

  if (deviceHandle == nullptr) {
    ESP_LOGE(TAG, "device handle is null");
  }

  hw::I2c i2c;
  common::Error errorCode = i2c.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "i2c init fail");
  }

  Sht40 sht40{i2c};
  errorCode = sht40.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "sht init fail");
  }

  timer::hw::HrTimer measurementTimer;
  errorCode = measurementTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "Measurement timer init fail");
  }

  app::SensorsController sensorsController{{measurementTimer, sht40, sht40}};
  sensorsController.start(MEASUREMENT_TIME_US);

  while (1) {
    sensorsController.yield();
    sw::delayMs(1000);
  }
}
}
