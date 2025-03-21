#include "sht40.hpp"

#include <array>

namespace sensor {
Sht40::Sht40(hw::II2c& i2c) : i2c_{i2c} {}

common::Error Sht40::init() {
  common::Error errorCode = i2c_.write(ADDRESS, RESET_COMMAND);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  sw::delayMs(10);
  return common::Error::OK;
}

void Sht40::setPrecision(const sht40::Precision precision) {
  precision_ = precision;
}

float Sht40::getTemperatureC() {
  return takeMeasurement_(Parameter::TEMPERATURE);
}

float Sht40::getHumidityRh() { return takeMeasurement_(Parameter::HUMIDITY); }

float Sht40::takeMeasurement_(Parameter parameter) {
  common::Error errorCode =
      i2c_.write(ADDRESS, static_cast<uint8_t>(precision_));
  if (errorCode != common::Error::OK) {
    return INVALID_VALUE;
  }
  sw::delayMs(20);

  std::array<uint8_t, BUFFER_SIZE> buffer{};
  errorCode = i2c_.read(ADDRESS, buffer.data(), buffer.size());
  if (errorCode != common::Error::OK) {
    return INVALID_VALUE;
  }

  uint16_t data{};
  if (parameter == Parameter::TEMPERATURE) {
    data = (buffer[0] << 8) | buffer[1];
    return calculateTemperatureC_(data);
  }

  data = (buffer[3] << 8) | buffer[4];
  return calculateHumidityRh_(data);
}

float Sht40::calculateTemperatureC_(const uint16_t data) {
  return -45.0f + 175.0f * static_cast<float>(data) / 65535.0f;
}

float Sht40::calculateHumidityRh_(const uint16_t data) {
  return -6.0f + 125.0f * static_cast<float>(data) / 65535.0f;
}

} // namespace sensor
