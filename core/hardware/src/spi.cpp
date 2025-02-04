#include "spi.hpp"

#include "driver/spi_common.h"
#include "driver/spi_master.h"

namespace hw {
Spi::Spi(Config config) : config_{config} {}

common::Error Spi::init(const spi::DmaChannel dmaChannel) {
  if (config_.mosi.isPinAssigned() == false or
      config_.miso.isPinAssigned() == false or
      config_.sck.isPinAssigned() == false) {
    return common::Error::FAIL;
  }

  int maxTransferSize{0};
  if (dmaChannel == spi::DmaChannel::DISABLED) {
    maxTransferSize = SOC_SPI_MAXIMUM_BUFFER_SIZE;
  } else {
    maxTransferSize = DMA_ENABLE;
  }

  spi_bus_config_t busConfig{};
  busConfig.miso_io_num = static_cast<int>(config_.miso.getPin());
  busConfig.mosi_io_num = static_cast<int>(config_.mosi.getPin());
  busConfig.sclk_io_num = static_cast<int>(config_.sck.getPin());
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

common::Error Spi::addDevice(spi::DeviceHandle& deviceHandle, IGpio& csPin,
                             const int clockSpeedHz) {
  if (csPin.isPinAssigned() == false) {
    return common::Error::INVALID_ARG;
  }

  spi_device_interface_config_t deviceConfig{};
  deviceConfig.clock_speed_hz = clockSpeedHz;
  deviceConfig.spics_io_num = static_cast<int>(csPin.getPin());
  deviceConfig.queue_size = QUEUE_SIZE;
  deviceConfig.command_bits = COMMAND_BITS;
  deviceConfig.address_bits = ADDRESS_BITS;
  deviceConfig.dummy_bits = DUMMY_BITS;
  deviceConfig.mode = CPOL_0_CPHA_0;

  esp_err_t espErrorCode = spi_bus_add_device(
      static_cast<spi_host_device_t>(config_.host), &deviceConfig,
      reinterpret_cast<spi_device_handle_t*>(&deviceHandle));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Spi::write(spi::DeviceHandle& deviceHandle, int registerAddress,
                         const uint8_t* buffer, size_t bufferLength) {
  spi_transaction_t t = {};
  t.addr = registerAddress;
  t.rx_buffer = NULL;
  t.tx_buffer = buffer;
  t.rxlength = bufferLength * 8;
  t.length = bufferLength * 8;

  esp_err_t espErrorCode =
      spi_device_polling_transmit((spi_device_handle_t)deviceHandle, &t);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Spi::read(spi::DeviceHandle& deviceHandle, int registerAddress,
                        uint8_t* buffer, size_t bufferLength) {
  spi_transaction_t t = {};
  t.addr = registerAddress;
  t.rx_buffer = buffer;
  t.tx_buffer = NULL;
  t.rxlength = bufferLength * 8;
  t.length = bufferLength * 8;

  esp_err_t espErrorCode =
      spi_device_polling_transmit((spi_device_handle_t)deviceHandle, &t);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}
} // namespace hw
