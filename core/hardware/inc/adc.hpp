#pragma once

#include "interfaces/iadc.hpp"
#include "interfaces/igpio.hpp"
#include "types.hpp"

namespace hw {
namespace adc {
enum class BitWidth : uint8_t {
  DEFAULT = 0,
  BITWIDTH_9 = 9,
  BITWIDTH_10,
  BITWIDTH_11,
  BITWIDTH_12,
  BITWIDTH_13
};
enum class Attenuation : uint8_t { DB_0 = 0, DB_2_5, DB_6, DB_11 };
} // namespace adc

class Adc : public IAdc {
  public:
    explicit Adc(IGpio& adcGpio);

    common::Error init(const adc::BitWidth bitWidth,
                       const adc::Attenuation attenuation);

    float readVoltage() override;

    common::Error deinit();

  private:
    using AdcHandle = void*;
    enum class UlpMode : uint8_t { DISABLE = 0, FSM, RISCV };
    enum class Unit : int8_t { UNKNOWN = -1, UNIT_1 = 0, UNIT_2 = 1 };
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
          if (unit == config.unit && channel == config.channel) {
            return true;
          }
          return false;
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
    static const Config INVALID_CONFIG;
    static std::vector<Config> usedConfig_;
    static AdcHandle adcUnit1Handle_;
    static AdcHandle adcUnit2Handle_;
    AdcHandle adcHandle_{nullptr};
    Config config_{Unit::UNKNOWN, Channel::UNKNOWN, adc::BitWidth::DEFAULT};
};
} // namespace hw
