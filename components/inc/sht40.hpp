#pragma once

#include <limits>

#include "delay.hpp"
#include "interfaces/ii2c.hpp"
#include "interfaces/isensors.hpp"
#include "types.hpp"

class Sht40 final : public ITemperatureSensor, public IHumiditySensor {
  public:
    /**
     * @brief Sht40 precision.
     */
    enum class Precision { HIGH = 0xFD, MEDIUM = 0xF6, LOW = 0xE0 };

    /**
     * @brief Construct a new Sht40 object.
     *
     * @param i2c I2C.
     */
    Sht40(hw::II2c& i2c);

    /**
     * @brief Initialize Sht40.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init();

    /**
     * @brief Set precision.
     *
     * @param precision Precision.
     */
    void setPrecision(const Precision precision);

    /**
     * @brief Get temperature in Celsius.
     *
     * @return
     *   - Temperature in Celsius.
     *   - INVALID_VALUE: Fail.
     */
    float getTemperatureC() override;

    /**
     * @brief Get humidity in RH.
     *
     * @return
     *   - Humidity in RH.
     *   - INVALID_VALUE: Fail.
     */
    float getHumidityRh() override;

  private:
    /**
     * @brief Sht40 parameter.
     */
    enum class Parameter {
      TEMPERATURE,
      HUMIDITY,
    };

    /**
     * @brief Take measurement.
     *
     * @param parameter Parameter.
     *
     * @return
     *    - Temperature in Celsius or Humidity in RH.
     *    - INVALID_VALUE: Fail.
     */
    float takeMeasurement_(Parameter parameter);

    /**
     * @brief Calculate temperature in Celsius.
     *
     * @param data Data.
     *
     * @return Temperature in Celsius.
     */
    float calculateTemperatureC_(const uint16_t data);

    /**
     * @brief Calculate humidity in RH.
     *
     * @param data Data.
     *
     * @return Humidity in RH.
     */
    float calculateHumidityRh_(const uint16_t data);

    static constexpr uint8_t ADDRESS{0x44};
    static constexpr uint8_t RESET_COMMAND{0x94};
    static constexpr size_t BUFFER_SIZE{6};
    static constexpr float INVALID_VALUE{std::numeric_limits<float>::max()};

    hw::II2c& i2c_;
    Precision precision_{Precision::HIGH};
};
