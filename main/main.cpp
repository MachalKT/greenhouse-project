#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "i2c.hpp"
#include "sht40.hpp"

const char* TAG = "MAIN";

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

    float temperatureC{};
    float humidityRh{};

    while(1) {
        temperatureC = sht40.getTemperatureC();
        humidityRh = sht40.getHumidityRh();
        ESP_LOGI(TAG, "temperature: %.2f, humidity: %.2f", temperatureC, humidityRh);
        core::sw::delayMs(1000);
    }
}
}
