#pragma once

#include <cstdint>
#include <cstddef>
#include "types.hpp"

namespace hw {
class II2c {
  public:
    virtual common::Error write(const uint8_t deviceAddress, const uint8_t command) = 0;

    virtual common::Error write(const uint8_t deviceAddress, const uint8_t registerAddress, const uint8_t* buffer, const size_t bufferLength) = 0;

    virtual common::Error read(const uint8_t deviceAddress, uint8_t* buffer, const size_t bufferLength) = 0;
};
} //hw
