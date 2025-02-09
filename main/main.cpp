#include <stdio.h>

#include "adc.hpp"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "gpio.hpp"
#include "hrtimer.hpp"
#include "i2c.hpp"
#include "sht40.hpp"
#include "spi.hpp"
#include "timedmeter.hpp"
#include "utils.hpp"

namespace {
const char* TAG = "MAIN";
static constexpr common::Time MEASUREMENT_TIME_US{
    common::utils::msToUs<common::Time, common::Time>(
        common::utils::sToMs<common::Time, common::Time>(1))};
} // namespace

extern "C" {
void app_main(void) {
  common::Error errorCode{common::Error::OK};

  hw::Gpio miso{19};
  hw::Gpio mosi{23};
  hw::Gpio sck{18};
  hw::Spi spi{{miso, mosi, sck, hw::spi::Host::VSPI}};

  errorCode = spi.init(hw::spi::DmaChannel::CHANNEL_1);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "spi init fail");
  }

  hw::I2c i2c;
  errorCode = i2c.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "i2c init fail");
  }

  hw::Gpio batteryGpio{32};
  hw::Adc adc{batteryGpio};
  errorCode =
      adc.init(hw::adc::BitWidth::BITWIDTH_12, hw::adc::Attenuation::DB_11);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "adc init fail");
  }

  sensor::Sht40 sht40{i2c};
  errorCode = sht40.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "sht init fail");
  }

  timer::hw::HrTimer measurementTimer;
  errorCode = measurementTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "Measurement timer init fail");
  }

  app::TimedMeter timedMeter{{measurementTimer, sht40, sht40}};
  timedMeter.start(MEASUREMENT_TIME_US);

  while (1) {
    timedMeter.yield();
    sw::delayMs(10);
  }
}
}
