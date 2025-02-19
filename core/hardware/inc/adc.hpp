/**
 * @file adc.hpp
 * @brief Definition of the Adc class for handling the Analog-to-Digital
 * Converter (ADC).
 */

#pragma once

#include "interfaces/iadc.hpp"
#include "interfaces/igpio.hpp"
#include "types.hpp"

#include <vector>

namespace hw {
namespace adc {

/**
 * @enum BitWidth
 * @brief Specifies the ADC output bit width.
 */
enum class BitWidth : uint8_t {
  DEFAULT = 0, // Default ADC output bit width, the maximum available width is
               // selected.
  BITWIDTH_9 = 9, // 9-bit ADC output width.
  BITWIDTH_10,    // 10-bit ADC output width.
  BITWIDTH_11,    // 11-bit ADC output width.
  BITWIDTH_12,    // 12-bit ADC output width.
  BITWIDTH_13     // 13-bit ADC output width.
};

/**
 * @enum Attenuation
 * @brief Specifies the input signal attenuation for ADC.
 */
enum class Attenuation : uint8_t {
  DB_0 = 0, // No input signal attenuation.
  DB_2_5,   // 2.5 dB attenuation.
  DB_6,     // 6 dB attenuation.
  DB_11     // 11 dB attenuation.
};
} // namespace adc

/**
 * @brief Class for handling the ADC.
 *
 * @note @brief By default, Ulp mode is disabled. To enable it, modify the
 * following variables according to the corresponding ADC unit:
 * - ULP_MODE_ADC_UNIT_1 for ADC Unit 1
 * - ULP_MODE_ADC_UNIT_2 for ADC Unit 2
 *
 * The maximum measurable input voltage, related to ADC attenuation, is set
 * to 3.3V by default. If necessary, modify the following variable:
 * - MAX_MEASURABLE_INPUT_V
 */
class Adc : public IAdc {
  public:
    /**
     * @brief Constructor of the Adc class.
     * @param adcGpio Reference to an IGpio object representing the ADC pin.
     */
    explicit Adc(IGpio& adcGpio);

    /**
     * @brief Initialize ADC
     * @param bitWidth ADC bit width.
     * @param attenuation Input signal attenuation.
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Wrong Gpio in Adc constructor
     */
    common::Error init(const adc::BitWidth bitWidth,
                       const adc::Attenuation attenuation);

    /**
     * @brief Reads voltage from the ADC.
     * @return Voltage value in volts.
     */
    float readVoltage() override;

    /**
     * @brief de-initialization ADC
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Adc has not been initialized
     */
    common::Error deinit();

  private:
    using AdcHandle = void*;

    /**
     * @enum UlpMode
     * @brief Ultra-Low-Power mode for ADC.
     */
    enum class UlpMode : uint8_t { DISABLE = 0, FSM, RISCV };

    /**
     * @enum Unit
     * @brief Specifies the ADC unit.
     */
    enum class Unit : int8_t { UNKNOWN = -1, UNIT_1 = 0, UNIT_2 = 1 };

    /**
     * @enum Channel
     * @brief Specifies the available ADC channels.
     */
    enum class Channel : int8_t {
      UNKNOWN = -1,
      CHANNEL_0 = 0,
      CHANNEL_1,
      CHANNEL_2,
      CHANNEL_3,
      CHANNEL_4,
      CHANNEL_5,
      CHANNEL_6,
      CHANNEL_7,
      CHANNEL_8,
      CHANNEL_9
    };

    /**
     * @struct Config
     * @brief Structure storing ADC configuration.
     */
    struct Config {
        Unit unit;
        Channel channel;
        adc::BitWidth bitWidth;

        void operator=(const Config& config) {
          unit = config.unit;
          channel = config.channel;
          bitWidth = config.bitWidth;
        }

        bool operator==(const Config& config) const {
          return unit == config.unit && channel == config.channel;
        }
    };

    void setConfig_(IGpio& adcGpio);
    void setUnitAndChannel_(const gpio::Number gpioNumber);
    void setAdcHandles_(AdcHandle& adcHandle, AdcHandle& adcUnitHandle,
                        const UlpMode ulpMode);

    common::Error setChannelConfiguration_(const adc::BitWidth bitWidth,
                                           const adc::Attenuation attenuation);
    bool hasUnit_(const Unit unit) const;
    common::Error clearAdcUnitHandle_(const Unit unit);

    static constexpr UlpMode ULP_MODE_ADC_UNIT_1{UlpMode::DISABLE};
    static constexpr UlpMode ULP_MODE_ADC_UNIT_2{UlpMode::DISABLE};
    static constexpr float MAX_MEASURABLE_INPUT_V{3.3f};
    static const Config INVALID_CONFIG;
    static std::vector<Config> usedConfig_;
    static AdcHandle adcUnit1Handle_;
    static AdcHandle adcUnit2Handle_;
    AdcHandle adcHandle_{nullptr};
    Config config_{Unit::UNKNOWN, Channel::UNKNOWN, adc::BitWidth::DEFAULT};
};
} // namespace hw
