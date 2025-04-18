#include "spi.hpp"

#include "driver/spi_common.h"
#include "driver/spi_master.h"

namespace hw {
Spi::Spi(Config config) : config_{config} {}

common::Error Spi::init(const DmaChannel dmaChannel) {
  if (config_.mosi.isGpioAssigned() == false or
      config_.miso.isGpioAssigned() == false or
      config_.sck.isGpioAssigned() == false) {
    return common::Error::FAIL;
  }

  int maxTransferSize{0};
  if (dmaChannel == DmaChannel::DISABLED) {
    maxTransferSize = SOC_SPI_MAXIMUM_BUFFER_SIZE;
  } else {
    maxTransferSize = DMA_ENABLE;
  }

  spi_bus_config_t busConfig{};
  busConfig.miso_io_num = static_cast<int>(config_.miso.getNumber());
  busConfig.mosi_io_num = static_cast<int>(config_.mosi.getNumber());
  busConfig.sclk_io_num = static_cast<int>(config_.sck.getNumber());
  busConfig.quadwp_io_num = NOT_USE_WRITE_PROTECT;
  busConfig.quadhd_io_num = NOT_USE_HOLD;
  busConfig.max_transfer_sz = maxTransferSize;

  esp_err_t espErrorCode =
      spi_bus_initialize(static_cast<spi_host_device_t>(config_.host),
                         &busConfig, static_cast<spi_dma_chan_t>(dmaChannel));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

SpiDeviceHandle Spi::addDevice(IGpio& csPin, const int clockSpeedHz) {
  if (csPin.isGpioAssigned() == false) {
    return nullptr;
  }

  spi_device_handle_t handle{nullptr};
  spi_device_interface_config_t deviceConfig{};
  deviceConfig.clock_speed_hz = clockSpeedHz;
  deviceConfig.spics_io_num = static_cast<int>(csPin.getNumber());
  deviceConfig.queue_size = QUEUE_SIZE;
  deviceConfig.command_bits = COMMAND_BITS;
  deviceConfig.address_bits = ADDRESS_BITS;
  deviceConfig.dummy_bits = DUMMY_BITS;
  deviceConfig.mode = CPOL_0_CPHA_0;

  esp_err_t espErrorCode = spi_bus_add_device(
      static_cast<spi_host_device_t>(config_.host), &deviceConfig, &handle);
  if (espErrorCode != ESP_OK) {
    return nullptr;
  }

  return static_cast<SpiDeviceHandle>(handle);
}

common::Error Spi::write(SpiDeviceHandle& deviceHandle,
                         const uint8_t registerAddress, const uint8_t* buffer,
                         const size_t bufferLength) {
  const size_t lengthInBits{bufferLength * 8};

  spi_transaction_t transaction{};
  transaction.addr = static_cast<uint64_t>(registerAddress);
  transaction.rx_buffer = nullptr;
  transaction.tx_buffer = buffer;
  transaction.rxlength = lengthInBits;
  transaction.length = lengthInBits;

  esp_err_t espErrorCode = spi_device_polling_transmit(
      (spi_device_handle_t)deviceHandle, &transaction);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Spi::read(SpiDeviceHandle& deviceHandle,
                        const uint8_t registerAddress, uint8_t* buffer,
                        const size_t bufferLength) {
  const size_t lengthInBits{bufferLength * 8};

  spi_transaction_t transaction{};
  transaction.addr = static_cast<uint64_t>(registerAddress);
  transaction.rx_buffer = buffer;
  transaction.tx_buffer = nullptr;
  transaction.rxlength = lengthInBits;
  transaction.length = lengthInBits;

  esp_err_t espErrorCode = spi_device_polling_transmit(
      (spi_device_handle_t)deviceHandle, &transaction);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}
} // namespace hw
