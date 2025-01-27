#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c.hpp"
#include "sht40.hpp"
#include "hrtimer.hpp"
#include "utils.hpp"
#include "sensorscontroller.hpp"

namespace {
const char* TAG = "MAIN";
static constexpr uint32_t MEASUREMENT_TIME_US{common::utils::msToUs<uint32_t, uint32_t>(common::utils::sToMs<uint32_t, uint32_t>(1))};
}

extern "C" {
void app_main(void)
{
    hw::I2c i2c;
    common::Error errorCode = i2c.init();
    if(errorCode != common::Error::OK) {
        ESP_LOGE(TAG, "i2c init fail");
    }

    Sht40 sht40{i2c};
    errorCode = sht40.init();
    if(errorCode != common::Error::OK) {
        ESP_LOGE(TAG, "sht init fail");
    }

    timer::hw::HrTimer measurementTimer;
    errorCode = measurementTimer.init();
    if(errorCode != common::Error::OK) {
        ESP_LOGE(TAG, "Measurement timer init fail");
    }

    app::SensorsController sensorsController{{measurementTimer, sht40, sht40}};
    sensorsController.start(MEASUREMENT_TIME_US);

    while(1) {
        sensorsController.yield();
        core::sw::delayMs(1000);
    }
}
}
