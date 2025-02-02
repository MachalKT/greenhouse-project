#pragma once

#include "types.hpp"

namespace hw {
namespace spi {
using DeviceHandle = void*;
}

class ISpi {
  public:
    virtual common::Error addDevice(spi::DeviceHandle& deviceHandle,
                                    const common::PinNumber csPin,
                                    const int clockSpeedHz) = 0;

    virtual common::Error write(spi::DeviceHandle& deviceHandle,
                                int registerAddress, const uint32_t* data,
                                size_t dataLength) = 0;

    virtual common::Error write(spi::DeviceHandle& deviceHandle,
                                int registerAddress, const uint8_t* buffer,
                                size_t bufferLength) = 0;

    virtual common::Error read(spi::DeviceHandle& deviceHandle,
                               int registerAddress, uint32_t* data,
                               size_t dataLength) = 0;

    virtual common::Error read(spi::DeviceHandle& deviceHandle,
                               int registerAddress, uint8_t* buffer,
                               size_t bufferLength) = 0;
};
} // namespace hw
