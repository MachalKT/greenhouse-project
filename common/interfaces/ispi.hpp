#pragma once

#include "interfaces/igpio.hpp"
#include "types.hpp"
#include <cstddef>

#include <cstddef>

namespace hw {
using SpiDeviceHandle = void*;

class ISpi {
  public:
    virtual common::Error write(SpiDeviceHandle& deviceHandle,
                                const uint8_t registerAddress,
                                const uint8_t* buffer,
                                const size_t bufferLength) = 0;

    virtual common::Error read(SpiDeviceHandle& deviceHandle,
                               const uint8_t registerAddress, uint8_t* buffer,
                               const size_t bufferLength) = 0;
};
} // namespace hw
