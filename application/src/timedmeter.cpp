#include "timedmeter.hpp"

#include "esp_log.h"

namespace {
const char* TAG = "TimedMeter";
}

namespace app {
TimedMeter::TimedMeter(Config config) : config_{config} {
  config.measurementTimer.setCallback(
      [](void* arg) {
        if (not arg) {
          return;
        }

        TimedMeter* sensorController = static_cast<TimedMeter*>(arg);
        sensorController->isReadyToMeasure_ = true;
      },
      this);
}

common::Error TimedMeter::start(common::Time timeUs) {
  return config_.measurementTimer.startPeriodic(timeUs);
}

common::Error TimedMeter::stop() { return config_.measurementTimer.stop(); }

common::MeasurementData TimedMeter::getMeasurementData() {
  return measurementValues_;
}

void TimedMeter::yield() {
  if (isReadyToMeasure_) {
    isReadyToMeasure_ = false;
    takeMeasurement_();
  }
}

void TimedMeter::takeMeasurement_() {
  measurementValues_.temperatureC = config_.temperatureSensor.getTemperatureC();
  measurementValues_.humidityRh = config_.humiditySensor.getHumidityRh();

  ESP_LOGI(TAG, "temperature: %.2f [C], humidity: %.2f [RH]",
           measurementValues_.temperatureC, measurementValues_.humidityRh);
}

} // namespace app
