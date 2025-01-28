#include "sensorscontroller.hpp"
#include "esp_log.h"

namespace {
const char* TAG = "SensorsController";
}

namespace app {
SensorsController::SensorsController(Config config) : config_{config} {
    config.measurementTimer.setCallback([](void* arg) {
        if(not arg) {
            return;
        }

        SensorsController* sensorController = static_cast<SensorsController*>(arg);
        sensorController->isReadyToMeasure_ = true;
    }, this);
}

common::Error SensorsController::start(common::Time timeUs) {
    return config_.measurementTimer.startPeriodic(timeUs);
}

common::Error SensorsController::stop() {
    return config_.measurementTimer.stop();
}

common::MeasurementValues SensorsController::getMeasurementValues() {
    return measurementValues_;
}

void SensorsController::yield() {
    if(isReadyToMeasure_) {
        isReadyToMeasure_ = false;
        takeMeasurement();
    }
}

void SensorsController::takeMeasurement() {
    measurementValues_.temperatureC = config_.temperatureSensor.getTemperatureC();
    measurementValues_.humidityRh = config_.humiditySensor.getHumidityRh();

    ESP_LOGI(TAG, "temperature: %.2f [C], humidity: %.2f [RH]", measurementValues_.temperatureC, measurementValues_.humidityRh);
}

} //app
