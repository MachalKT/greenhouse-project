#include <stdio.h>

#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hrtimer.hpp"
#include "i2c.hpp"
#include "sensorscontroller.hpp"
#include "sht40.hpp"
#include "utils.hpp"

namespace {
const char* TAG = "MAIN";
static constexpr common::Time MEASUREMENT_TIME_US{
    common::utils::msToUs<common::Time, common::Time>(
        common::utils::sToMs<common::Time, common::Time>(1))};
} // namespace

extern "C" {
void app_main(void) {

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
