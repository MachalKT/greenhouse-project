#pragma once

#include <limits>

#include "delay.hpp"
#include "interfaces/ii2c.hpp"
#include "interfaces/isensors.hpp"
#include "types.hpp"

class Sht40 final : public ITemperatureSensor, public IHumiditySensor {
  public:
    enum class Precision { HIGH = 0xFD, MEDIUM = 0xF6, LOW = 0xE0 };

    Sht40(hw::II2c& i2c);

    common::Error init();

    void setPrecision(const Precision precision);

    float getTemperatureC() override;

    float getHumidityRh() override;

  private:
    enum class Parameter {
      TEMPERATURE,
      HUMIDITY,
    };

    float takeMeasurement(Parameter parameter);

    float calculateTemperatureC(const uint16_t data);

    float calculateHumidityRh(const uint16_t data);

    static constexpr uint8_t ADDRESS {0x44};
    static constexpr uint8_t RESET_COMMAND {0x94};
    static constexpr size_t BUFFER_SIZE {6};
    static constexpr float INVALID_VALUE {std::numeric_limits<float>::max()};

    hw::II2c& i2c_;
    Precision precision_ {Precision::HIGH};
};
