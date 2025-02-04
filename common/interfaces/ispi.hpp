#pragma once

#include "interfaces/igpio.hpp"
#include "types.hpp"

namespace hw {
namespace spi {
using DeviceHandle = void*;
}

class ISpi {
  public:
    virtual common::Error addDevice(spi::DeviceHandle& deviceHandle,
                                    IGpio& csPin, const int clockSpeedHz) = 0;

    virtual common::Error write(spi::DeviceHandle& deviceHandle,
                                int registerAddress, const uint8_t* buffer,
                                size_t bufferLength) = 0;

    virtual common::Error read(spi::DeviceHandle& deviceHandle,
                               int registerAddress, uint8_t* buffer,
                               size_t bufferLength) = 0;
};
} // namespace hw
