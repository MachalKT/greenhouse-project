#pragma once

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "interfaces/ii2c.hpp"

namespace hw {
class I2c final : public II2c {
  public:
    common::Error init();

    common::Error write(const uint8_t deviceAddress,
                        const uint8_t command) override;

    common::Error write(const uint8_t deviceAddress,
                        const uint8_t registerAddress, const uint8_t* buffer,
                        const size_t bufferLength) override;

    common::Error read(const uint8_t deviceAddress, uint8_t* buffer,
                       const size_t bufferLength) override;

  private:
    const i2c_port_t I2C_PORT {I2C_NUM_1};
    const i2c_mode_t I2C_MODE {I2C_MODE_MASTER};
    const gpio_num_t I2C_SDA_IO_NUM {GPIO_NUM_21};
    const int I2C_SCL_IO_NUM {22};
    const gpio_pullup_t I2C_SDA_PULLUP_EN {GPIO_PULLUP_ENABLE};
    const gpio_pullup_t I2C_SCL_PULLUP_EN {GPIO_PULLUP_ENABLE};
    const uint32_t I2C_CLOCK_SPEED {400'000};
};
} // namespace hw
