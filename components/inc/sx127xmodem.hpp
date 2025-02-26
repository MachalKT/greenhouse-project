// Copyright 2023
//
// Library for sx127xmodem modified and adapted to the project based on
// the library for sx127x.
// Creator of the sx127x library:
// Andrey Rodionov <dernasherbrezon@gmail.com>
// The spi sx127x library can be obtained at
//
//      https://github.com/dernasherbrezon/sx127x/tree/main
//

// Copyright 2022 Andrey Rodionov <dernasherbrezon@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "ispi.hpp"
#include "types.hpp"

namespace sx127x {
/**
 * @brief Used to change the operating mode
 */
enum class Mode : uint8_t {
  SLEEP = 0b00000000,     // SLEEP
  STANDBY = 0b00000001,   // STDBY
  FSTX = 0b00000010,      // Frequency synthesis TX
  TX = 0b00000011,        // Transmit
  FSRX = 0b00000100,      // Frequency synthesis RX
  RX_CONT = 0b00000101,   // Receive continuous
  RX_SINGLE = 0b00000110, // Receive single
  CAD = 0b00000111        // Channel activity detection
};

enum Modulation : uint8_t {
  LORA = 0b10000000,
  FSK = 0b00000000, // default
  OOK = 0b00100000
};

/**
 * @brief Used to change gain
 */
enum class Gain : uint8_t {
  G1 = 0b00100000, // Maximum gain
  G2 = 0b01000000,
  G3 = 0b01100000,
  G4 = 0b10000000,
  G5 = 0b10100000,
  G6 = 0b11000000,  // Minimum gain
  AUTO = 0b00000000 // Automatic. See 5.5.3. for details
};

/**
 * @brief Signal bandwidth.
 * @note In the lower band (169MHz), signal bandwidths 8&9 (250k and 500k)
 * are not supported
 */
enum class Bandwidth : uint8_t {
  BW_7800 = 0b00000000,
  BW_10400 = 0b00010000,
  BW_15600 = 0b00100000,
  BW_20800 = 0b00110000,
  BW_31250 = 0b01000000,
  BW_41700 = 0b01010000,
  BW_62500 = 0b01100000,
  BW_125000 = 0b01110000, // default
  BW_250000 = 0b10000000,
  BW_500000 = 0b10010000
};

enum class HeaderMode : uint8_t {
  EXPLICIT = 0b00000000,
  IMPLICIT = 0b00000001
};

/**
 * @brief SF rate (expressed as a base-2 logarithm)
 */
enum class SF {
  SF_6 = 0b01100000,  // 64 chips / symbol  don't have to use
  SF_7 = 0b01110000,  // 128 chips / symbol
  SF_8 = 0b10000000,  // 256 chips / symbol
  SF_9 = 0b10010000,  // 512 chips / symbol
  SF_10 = 0b10100000, // 1024 chips / symbol
  SF_11 = 0b10110000, // 2048 chips / symbol
  SF_12 = 0b11000000  // 4096 chips / symbol
};

/**
 * @brief Type of interrupt write to register
 */
enum class DioMapping1 : uint8_t {
  RX_DONE = 0b00000000, // Packet reception complete
  TX_DONE = 0b01000000  // FIFO Payload transmission complete
};

/**
 * @brief sx127x chip has 2 pins for TX. Based on the pin below chip can
 * produce different max power.
 */
enum class PaPin : uint8_t {
  RFO = 0b00000000,  // RFO pin. Output power is limited to +14 dBm.
  BOOST = 0b10000000 // PA_BOOST pin. Output power is limited to +20 dBm
};

class ModemBase {
  public:
    struct BaseConfig {
        hw::ISpi& spi;
        hw::SpiDeviceHandle& spiHandle;
        Modulation modulation;
    };

    ModemBase(BaseConfig config);

    common::Error setMode(Mode mode);

    common::Error setFrequency(uint64_t frequencyHz);

    uint64_t getFrequencyHz();

    virtual common::Error resetFifo() = 0;

    common::Error setLnaBoostHf(bool enable);

    virtual common::Error setLnaGain(Gain gain);

    virtual common::Error setBandwidth(Bandwidth bandwidth) = 0;

    virtual common::Error setHeader() = 0;

    virtual common::Error setModemConfig2(SF spreadingFactor) = 0;

    virtual common::Error setSyncWord(uint8_t value) = 0;

    virtual common::Error setPreambleLength(uint16_t length) = 0;

    common::Error setPaConfig(PaPin pin, int8_t power);

    virtual common::Error getRxData(uint8_t* data, const size_t dataLength) = 0;

    virtual size_t getRxDataLength() = 0;

    common::Error listening();

    virtual common::Error transmitData(const uint8_t* data,
                                       const size_t dataLength);

    virtual common::SignalQuality getSignalQuality() = 0;

    virtual common::radio::IrqEvent getInterrupValue() = 0;

  protected:
    common::Error write_(uint8_t registerAddress, const uint8_t* buffer,
                         const size_t bufferLength);
    common::Error read_(uint8_t registerAddress, uint8_t* buffer,
                        const size_t bufferLength);

    common::Error appendRegister_(int reg, uint8_t& value, uint8_t mask);

    uint8_t overCurrentProtection_(PaPin pin, int8_t power);

    uint8_t paConfigValue_(PaPin pin, int8_t power);

    static constexpr uint64_t OSCILLATOR_FREQUENCY_HZ{32'000'000};
    static constexpr uint8_t FIFO_BASE_ADDRESS{0b00000000};
    BaseConfig config_;
};

class LoRa final : public ModemBase {
  public:
    struct Config {
        hw::ISpi& spi;
        hw::SpiDeviceHandle& spiHandle;
    };

    LoRa(Config config);

    common::Error resetFifo() override;

    common::Error setLnaGain(Gain gain) override;

    common::Error setBandwidth(Bandwidth bandwidth) override;

    common::Error setHeader() override;

    common::Error setModemConfig2(SF spreadingFactor) override;

    common::Error setSyncWord(uint8_t value) override;

    common::Error setPreambleLength(uint16_t length) override;

    common::Error getRxData(uint8_t* data, const size_t dataLength) override;

    size_t getRxDataLength() override;

    common::Error transmitData(const uint8_t* data,
                               const size_t dataLength) override;

    common::SignalQuality getSignalQuality() override;

    common::radio::IrqEvent getInterrupValue() override;

  private:
    common::Error reloadLowDatarateOptimization_();

    common::Error getBandwidth_(uint32_t* bandwidth);

    common::Error setLowDatarateOptimization_(bool enable);

    int16_t getRssi_();

    float getSnr_();

    static constexpr uint64_t RF_MID_BAND_THRESHOLD_HZ{525'000'000};
};

} // namespace sx127x