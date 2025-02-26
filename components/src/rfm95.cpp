#include "rfm95.hpp"
#include "delay.hpp"
#include "esp_log.h"
#include "sx127xregisters.hpp"
#include <array>

namespace radio {
Rfm95::Rfm95(Config config) : config_{config} {}

common::Error Rfm95::init(Modulation modulation) {
  common::Error errorCode = config_.reset.setMode(hw::GpioMode::OUTPUT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = config_.reset.setLevel(hw::GpioLevel::HIGH);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  sw::delayMs(20);

  errorCode = config_.dio0.setMode(hw::GpioMode::INPUT);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = config_.dio0.configurePullUpDown(false, true);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setModem_(modulation);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = modem_->setMode(Mode::SLEEP);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = modem_->resetFifo();
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return modem_->setMode(Mode::STANDBY);
}

common::Error Rfm95::send(const uint8_t* data, const size_t dataLength) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->transmitData(data, dataLength);
}

common::Error Rfm95::receive(uint8_t* data, const size_t dataLength) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->getRxData(data, dataLength);
}

size_t Rfm95::getReceiveDataLength() {
  if (modem_ == nullptr) {
    return 0;
  }

  return modem_->getRxDataLength();
}

common::Error Rfm95::listening() {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->listening();
}

common::Error Rfm95::setIrqEventCallback(common::Callback cb,
                                         common::Argument arg) {
  return config_.dio0.setInterrupt(hw::GpioInterruptType::RISING_EDGE, cb, arg);
}

common::radio::IrqEvent Rfm95::getIrqEvent() {
  if (modem_ == nullptr) {
    return common::radio::IrqEvent::UNKNOWN;
  }

  return modem_->getIrqEvent();
}

common::Error Rfm95::setAllSettings(ModemSettings settings) {
  common::Error errorCode = setFrequency(settings.frequencyHz);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setLnaBoostHf(settings.lnaBoostHfEnable);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setLnaGain(settings.gain);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setBandwidth(settings.bandwidth);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setHeader();
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setModemConfig(settings.spreadingFactor);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setSyncWord(settings.syncWord);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setPreambleLength(settings.preambleLength);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = setPaConfig(settings.paPin, settings.power);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Rfm95::setFrequency(uint64_t frequencyHz) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setFrequency(frequencyHz);
}

common::Error Rfm95::setLnaBoostHf(bool enable) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setLnaBoostHf(enable);
}

common::Error Rfm95::setLnaGain(Gain gain) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setLnaGain(gain);
}

common::Error Rfm95::setBandwidth(Bandwidth bandwidth) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setBandwidth(bandwidth);
}

common::Error Rfm95::setHeader() {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setHeader();
}

common::Error Rfm95::setModemConfig(SF spreadingFactor) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setModemConfig2(spreadingFactor);
}

common::Error Rfm95::setSyncWord(uint8_t value) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setSyncWord(value);
}

common::Error Rfm95::setPreambleLength(uint16_t length) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setPreambleLength(length);
}

common::Error Rfm95::setPaConfig(PaPin pin, int8_t power) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setPaConfig(pin, power);
}

common::SignalQuality Rfm95::getSignalQuality() {
  if (modem_ == nullptr) {
    return common::SignalQuality{common::SignalQuality::RSSI_INVALID_VALUE,
                                 common::SignalQuality::SNR_INVALID_VALUE};
  }

  return modem_->getSignalQuality();
}

common::Error Rfm95::setModem_(Modulation& modulation) {
  if (modulation != Modulation::LORA) {
    return common::Error::INVALID_ARG;
  }

  sx127x::LoRa::Config config = {config_.spi, config_.spiDeviceHandle};
  modem_ = std::make_unique<sx127x::LoRa>(config);

  return common::Error::OK;
}

common::Error Rfm95::setMode_(Mode mode) {
  if (modem_ == nullptr) {
    return common::Error::FAIL;
  }

  return modem_->setMode(mode);
}

} // namespace radio
