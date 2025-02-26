#include "sx127xmodem.hpp"
#include "delay.hpp"
#include "sx127xregisters.hpp"
#include <array>

namespace sx127x {
/**
 * ModemBase
 */
ModemBase::ModemBase(BaseConfig config) : config_{config} {}

common::Error ModemBase::setMode(Mode mode) {
  uint8_t value = (static_cast<uint8_t>(mode) | config_.modulation);

  return write_(reg::common::OP_MODE, &value, sizeof(value));
}

common::Error ModemBase::setFrequency(uint64_t frequencyHz) {
  uint64_t frf = (frequencyHz << 19) / OSCILLATOR_FREQUENCY_HZ;
  std::array<uint8_t, reg::size::FRF> frfData{static_cast<uint8_t>(frf >> 16),
                                              static_cast<uint8_t>(frf >> 8),
                                              static_cast<uint8_t>(frf >> 0)};

  return write_(reg::common::FRF_MSB, frfData.data(), frfData.size());
}

uint64_t ModemBase::getFrequencyHz() {
  std::array<uint8_t, reg::size::FRF> frfData{};
  common::Error errorCode =
      read_(reg::common::FRF_MSB, frfData.data(), frfData.size());
  if (errorCode != common::Error::OK) {
    return INVALID_FREQUENCY_HZ;
  }

  uint64_t frf = (static_cast<uint64_t>(frfData[0]) << 16) |
                 (static_cast<uint64_t>(frfData[1]) << 8) |
                 (static_cast<uint64_t>(frfData[2]) << 0);

  return (frf * OSCILLATOR_FREQUENCY_HZ) >> 19;
}

common::Error ModemBase::setLnaBoostHf(bool enable) {
  constexpr uint8_t LNA_BOOST_HF_ON{0b00000011};
  constexpr uint8_t LNA_BOOST_HF_OFF{0b00000000};
  uint8_t lnaBoostHf = enable ? LNA_BOOST_HF_ON : LNA_BOOST_HF_OFF;

  return appendRegister_(reg::common::LNA, lnaBoostHf,
                         reg::common::mask::LNA_BOOST_HF);
}

common::Error ModemBase::setLnaGain(Gain gain) {
  uint8_t gainValue{static_cast<uint8_t>(gain)};
  return appendRegister_(reg::common::LNA, gainValue,
                         reg::common::mask::LNA_GAIN);
}

common::Error ModemBase::setPaConfig(PaPin pin, int8_t power) {
  if (pin == PaPin::RFO && (power < -4 || power > 15)) {
    return common::Error::INVALID_ARG;
  }
  if (pin == PaPin::BOOST && (power < 2 || power > 20)) {
    return common::Error::INVALID_ARG;
  }

  constexpr uint8_t HIGH_POWER_ON{0b10000111};
  constexpr uint8_t HIGH_POWER_OFF{0b10000100};
  uint8_t highPower{0};
  if (pin == PaPin::BOOST && power == 20) {
    highPower = HIGH_POWER_ON;
  } else {
    highPower = HIGH_POWER_OFF;
  }

  common::Error errorCode =
      write_(reg::common::PA_DAC, &highPower, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t data = overCurrentProtection_(pin, power);
  errorCode = write_(reg::common::OCP, &data, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  data = paConfigValue_(pin, power);
  if (data == 0) {
    return common::Error::FAIL;
  }

  return write_(reg::common::PA_CONFIG, &data, reg::size::DEFAULT);
}

common::Error ModemBase::listening() {
  uint8_t dioMapping1 = static_cast<uint8_t>(DioMapping1::RX_DONE);
  common::Error errorCode =
      appendRegister_(reg::common::DIO_MAPPING_1, dioMapping1,
                      reg::common::mask::DIO_MAPPING_1);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return setMode(Mode::RX_CONT);
}

common::Error ModemBase::transmitData(const uint8_t* data,
                                      const size_t dataLength) {
  common::Error errorCode = write_(reg::common::FIFO, data, dataLength);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t dioMapping1 = static_cast<uint8_t>(DioMapping1::TX_DONE);
  errorCode = appendRegister_(reg::common::DIO_MAPPING_1, dioMapping1,
                              reg::common::mask::DIO_MAPPING_1);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return setMode(Mode::TX);
}

common::Error ModemBase::write_(uint8_t registerAddress, const uint8_t* buffer,
                                const size_t bufferLength) {
  registerAddress = registerAddress | 0x80;
  return config_.spi.write(config_.spiHandle, registerAddress, buffer,
                           bufferLength);
}

common::Error ModemBase::read_(uint8_t registerAddress, uint8_t* buffer,
                               const size_t bufferLength) {
  registerAddress = registerAddress & 0x7F;
  return config_.spi.read(config_.spiHandle, registerAddress, buffer,
                          bufferLength);
}

common::Error ModemBase::appendRegister_(int reg, uint8_t& value,
                                         uint8_t mask) {
  uint8_t previous = 0;
  common::Error errorCode = read_(reg, &previous, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t data = ((uint8_t)previous & mask) | value;
  return write_(reg, &data, reg::size::DEFAULT);
}

uint8_t ModemBase::overCurrentProtection_(PaPin pin, int8_t power) {
  uint8_t maxCurrent = 0;
  if (pin == PaPin::BOOST) {
    if (power == 20) {
      maxCurrent = 120;
    } else {
      maxCurrent = 87;
    }
  } else {
    if (power > 7) {
      maxCurrent = 29;
    } else {
      maxCurrent = 20;
    }
  }

  uint8_t data = (maxCurrent - 45) / 5;
  data += 0b00100000;
  return data;
}

uint8_t ModemBase::paConfigValue_(PaPin pin, int8_t power) {
  constexpr uint8_t MAX_POWER{0b01110000};
  constexpr uint8_t LOW_POWER{0b00000000};

  if (pin == PaPin::RFO) {
    if (power < 0) {
      return LOW_POWER | (power + 4);
    } else {
      return MAX_POWER | power;
    }
    return static_cast<uint8_t>(PaPin::RFO);
  } else {
    if (power == 20) {
      return static_cast<uint8_t>(PaPin::BOOST) | 0b00001111;
    } else {
      return static_cast<uint8_t>(PaPin::BOOST) | (power - 2);
    }
  }
  return 0;
}

/**
 * LoRa
 */
LoRa::LoRa(Config config)
    : ModemBase{{config.spi, config.spiHandle, Modulation::LORA}} {}

common::Error LoRa::resetFifo() {
  uint8_t fifoBaseAddress{FIFO_BASE_ADDRESS};

  common::Error errorCode = write_(reg::lora::FIFO_TX_BASE_ADDR,
                                   &fifoBaseAddress, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return write_(reg::lora::FIFO_RX_BASE_ADDR, &fifoBaseAddress,
                reg::size::DEFAULT);
}

common::Error LoRa::setLnaGain(Gain gain) {
  constexpr uint8_t AGC_ON{0b00000100};
  constexpr uint8_t AGC_OFF{0b00000000};
  uint8_t agc{0};

  if (gain == Gain::AUTO) {
    agc = AGC_ON;
    return appendRegister_(reg::lora::MODEM_CONFIG_3, agc,
                           reg::lora::mask::AGC);
  }

  agc = AGC_OFF;
  common::Error errorCode =
      appendRegister_(reg::lora::MODEM_CONFIG_3, agc, reg::lora::mask::AGC);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return ModemBase::setLnaGain(gain);
}

common::Error LoRa::setBandwidth(Bandwidth bandwidth) {
  uint8_t bandwidthValue{static_cast<uint8_t>(bandwidth)};

  common::Error errorCode = appendRegister_(
      reg::lora::MODEM_CONFIG_1, bandwidthValue, reg::lora::mask::BANDWIDTH);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return reloadLowDatarateOptimization_();
}

common::Error LoRa::setHeader() {
  uint8_t header{static_cast<uint8_t>(HeaderMode::EXPLICIT)};

  return appendRegister_(reg::lora::MODEM_CONFIG_1, header,
                         reg::lora::mask::HEADER);
}

common::Error LoRa::setModemConfig2(SF spreadingFactor) {
  if (spreadingFactor == SF::SF_6) {
    return common::Error::INVALID_ARG;
  }

  uint8_t detectionOptimize{0xc3};
  uint8_t detectionThreshold{0x0a};

  common::Error errorCode = write_(reg::lora::DETECT_OPTIMIZE,
                                   &detectionOptimize, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = write_(reg::lora::DETECTION_THRESHOLD, &detectionThreshold,
                     reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t spreadingFactorValue{static_cast<uint8_t>(spreadingFactor)};
  errorCode = appendRegister_(reg::lora::MODEM_CONFIG_2, spreadingFactorValue,
                              reg::lora::mask::SPREADING_FACTOR);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return reloadLowDatarateOptimization_();
}

common::Error LoRa::setSyncWord(uint8_t value) {
  return write_(reg::lora::SYNC_WORD, &value, reg::size::DEFAULT);
}

common::Error LoRa::setPreambleLength(uint16_t length) {
  std::array<uint8_t, reg::size::PREAMBLE> pramble{
      static_cast<uint8_t>(length >> 8), static_cast<uint8_t>(length >> 0)};

  return write_(reg::lora::PREAMBLE_MSB, pramble.data(), reg::size::PREAMBLE);
}

common::Error LoRa::getRxData(uint8_t* data, const size_t dataLength) {
  if (dataLength < getRxDataLength()) {
    return common::Error::INVALID_ARG;
  }

  uint8_t fifoRxAddress{0};
  common::Error errorCode = read_(reg::lora::FIFO_RX_CURRENT_ADDR,
                                  &fifoRxAddress, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode =
      write_(reg::lora::FIFO_ADDR_PTR, &fifoRxAddress, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  sw::delayMs(1); // delay because data cannot be read

  return read_(reg::common::FIFO, data, dataLength);
}

size_t LoRa::getRxDataLength() {
  uint8_t length{0};
  common::Error errorCode =
      read_(reg::lora::RX_NB_BYTES, &length, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return 0;
  }

  return length;
}

common::Error LoRa::transmitData(const uint8_t* data, const size_t dataLength) {
  uint8_t fifoAddress{FIFO_BASE_ADDRESS};

  common::Error errorCode =
      write_(reg::lora::FIFO_ADDR_PTR, &fifoAddress, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t payloadLength{static_cast<uint8_t>(dataLength)};

  errorCode =
      write_(reg::lora::PAYLOAD_LENGTH, &payloadLength, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  sw::delayMs(1); // delay because data cannot be write

  return ModemBase::transmitData(data, dataLength);
}

common::SignalQuality LoRa::getSignalQuality() {
  common::SignalQuality signalQuality;

  signalQuality.rssi = getRssi_();
  if (signalQuality.rssi == common::SignalQuality::RSSI_INVALID_VALUE) {
    signalQuality.snr = common::SignalQuality::SNR_INVALID_VALUE;
    return signalQuality;
  }

  signalQuality.snr = getSnr_();
  if (signalQuality.snr == common::SignalQuality::SNR_INVALID_VALUE) {
    return signalQuality;
  }

  if (signalQuality.snr < 0) {
    signalQuality.rssi = signalQuality.rssi + signalQuality.snr;
  }

  return signalQuality;
}

common::radio::IrqEvent LoRa::getIrqEvent() {
  uint8_t value{0};
  common::Error errorCode =
      read_(reg::lora::IRQ_FLAGS, &value, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::radio::IrqEvent::UNKNOWN;
  }

  errorCode = write_(reg::lora::IRQ_FLAGS, &value, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::radio::IrqEvent::UNKNOWN;
  }

  constexpr uint8_t IRQ_RX_DONE{0b01000000};
  constexpr uint8_t IRQ_TX_DONE{0b00001000};
  if (value & IRQ_RX_DONE) {
    return common::radio::IrqEvent::RX_DONE;
  } else if (value & IRQ_TX_DONE) {
    return common::radio::IrqEvent::TX_DONE;
  }

  return common::radio::IrqEvent::UNKNOWN;
}

common::Error LoRa::reloadLowDatarateOptimization_() {
  uint32_t bandwidth{0};
  common::Error errorCode = getBandwidth_(&bandwidth);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t data{0};
  errorCode = read_(reg::lora::MODEM_CONFIG_2, &data, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t spreadingFactor{data};
  spreadingFactor = (spreadingFactor >> 4);

  uint32_t symbolDuration = 1000 / (bandwidth / (1L << spreadingFactor));
  if (symbolDuration <= 16) {
    return common::Error::OK;
  }

  return setLowDatarateOptimization_(true);
}

common::Error LoRa::getBandwidth_(uint32_t* bandwidth) {
  uint8_t data{0};

  common::Error errorCode =
      read_(reg::lora::MODEM_CONFIG_1, &data, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  uint8_t config{data};
  config = (config >> 4);
  switch (config) {
  case 0b0000:
    *bandwidth = 7800;
    break;
  case 0b0001:
    *bandwidth = 10'400;
    break;
  case 0b0010:
    *bandwidth = 15'600;
    break;
  case 0b0011:
    *bandwidth = 20'800;
    break;
  case 0b0100:
    *bandwidth = 31'250;
    break;
  case 0b0101:
    *bandwidth = 41'700;
    break;
  case 0b0110:
    *bandwidth = 62'500;
    break;
  case 0b0111:
    *bandwidth = 125'000;
    break;
  case 0b1000:
    *bandwidth = 250'000;
    break;
  case 0b1001:
    *bandwidth = 500'000;
    break;
  default:
    return common::Error::FAIL;
  }
  return common::Error::OK;
}

common::Error LoRa::setLowDatarateOptimization_(bool enable) {
  constexpr uint8_t LOW_DATA_RATE_OPTIMIZE_ENABLE{0b00001000};
  constexpr uint8_t LOW_DATA_RATE_OPTIMIZE_DISABLE{0b00000000};

  uint8_t lowDataRateOptamize =
      enable ? LOW_DATA_RATE_OPTIMIZE_ENABLE : LOW_DATA_RATE_OPTIMIZE_DISABLE;

  return appendRegister_(reg::lora::MODEM_CONFIG_3, lowDataRateOptamize,
                         reg::lora::mask::LOW_DATA_RATE_OPTIMIZE);
}

int16_t LoRa::getRssi_() {
  uint8_t rssiValue{0};
  common::Error errorCode =
      read_(reg::lora::PKT_RSSI_VALUE, &rssiValue, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::SignalQuality::RSSI_INVALID_VALUE;
  }

  const uint64_t frequencyHz = getFrequencyHz();
  if (frequencyHz == 0) {
    return common::SignalQuality::RSSI_INVALID_VALUE;
  }

  constexpr uint64_t RF_MID_BAND_THRESHOLD_HZ{525'000'000};
  if (frequencyHz < RF_MID_BAND_THRESHOLD_HZ) {
    // section 5.5.5. low frequency
    return static_cast<int16_t>(-164) + static_cast<int16_t>(rssiValue);
  }

  // section 5.5.5. high frequency
  return static_cast<int16_t>(-157) + static_cast<int16_t>(rssiValue);
}

float LoRa::getSnr_() {
  uint8_t snrValue{0};
  common::Error errorCode =
      read_(reg::lora::PKT_SNR_VALUE, &snrValue, reg::size::DEFAULT);
  if (errorCode != common::Error::OK) {
    return common::SignalQuality::SNR_INVALID_VALUE;
  }

  return static_cast<float>(snrValue) * 0.25f;
}

} // namespace sx127x
