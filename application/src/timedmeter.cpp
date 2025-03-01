#include "timedmeter.hpp"

#include "esp_log.h"
#include <string_view>

namespace {
static constexpr std::string_view TAG{"TimedMeter"};
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
  takeMeasurement_();
  return config_.measurementTimer.startPeriodic(timeUs);
}

common::Error TimedMeter::stop() { return config_.measurementTimer.stop(); }

common::Telemetry TimedMeter::getMeasurementData() { return config_.telemetry; }

void TimedMeter::yield() {
  if (isReadyToMeasure_) {
    isReadyToMeasure_ = false;
    takeMeasurement_();
  }
}

void TimedMeter::takeMeasurement_() {
  config_.telemetry.temperatureC = config_.temperatureSensor.getTemperatureC();
  config_.telemetry.humidityRh = config_.humiditySensor.getHumidityRh();

  if (config_.telemetry.temperatureC == sensor::INVALID_VALUE ||
      config_.telemetry.humidityRh == sensor::INVALID_VALUE) {
    ESP_LOGE(TAG.data(), "Measurement fail");
  }

  ESP_LOGI(TAG.data(), "temperature: %.2f [C], humidity: %.2f [RH]",
           config_.telemetry.temperatureC, config_.telemetry.humidityRh);
}

} // namespace app
