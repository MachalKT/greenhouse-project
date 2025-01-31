#include "i2c.hpp"

namespace hw {
common::Error I2c::init() {
  i2c_config_t i2cConfig {};
  i2cConfig.mode = I2C_MODE;
  i2cConfig.sda_io_num = I2C_SDA_IO_NUM;
  i2cConfig.sda_pullup_en = I2C_SDA_PULLUP_EN;
  i2cConfig.scl_io_num = I2C_SCL_IO_NUM;
  i2cConfig.scl_pullup_en = I2C_SCL_PULLUP_EN;
  i2cConfig.master.clk_speed = I2C_CLOCK_SPEED;
  i2cConfig.clk_flags = 0;

  esp_err_t errorCode = i2c_param_config(I2C_PORT, &i2cConfig);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_driver_install(I2C_PORT, i2cConfig.mode, 0, 0, 0);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error I2c::write(const uint8_t deviceAddress, const uint8_t command) {
  i2c_cmd_handle_t cmdHandle = i2c_cmd_link_create();

  esp_err_t errorCode = i2c_master_start(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_write_byte(
      cmdHandle, (deviceAddress << 1) | I2C_MASTER_WRITE, true);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_write_byte(cmdHandle, command, true);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_stop(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode =
      i2c_master_cmd_begin(I2C_PORT, cmdHandle, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error I2c::write(const uint8_t deviceAddress,
                         const uint8_t registerAddress, const uint8_t* buffer,
                         const size_t bufferLength) {
  i2c_cmd_handle_t cmdHandle = i2c_cmd_link_create();

  esp_err_t errorCode = i2c_master_start(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_write_byte(
      cmdHandle, (deviceAddress << 1) | I2C_MASTER_WRITE, true);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_write_byte(cmdHandle, registerAddress, true);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_write(cmdHandle, buffer, bufferLength, true);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_stop(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode =
      i2c_master_cmd_begin(I2C_PORT, cmdHandle, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error I2c::read(const uint8_t deviceAddress, uint8_t* buffer,
                        const size_t bufferLength) {
  i2c_cmd_handle_t cmdHandle = i2c_cmd_link_create();

  esp_err_t errorCode = i2c_master_start(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_write_byte(
      cmdHandle, (deviceAddress << 1) | I2C_MASTER_READ, true);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode =
      i2c_master_read(cmdHandle, buffer, bufferLength, I2C_MASTER_LAST_NACK);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode = i2c_master_stop(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  errorCode =
      i2c_master_cmd_begin(I2C_PORT, cmdHandle, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmdHandle);
  if (errorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}
} // namespace hw
