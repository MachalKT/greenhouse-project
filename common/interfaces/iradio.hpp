#pragma once

#include "types.hpp"
#include <cstddef>
#include <cstdint>

namespace radio {
class IRadio {
  public:
    virtual common::Error send(const uint8_t* data,
                               const size_t dataLength) = 0;

    virtual common::Error receive(uint8_t* data, const size_t dataLength) = 0;

    virtual size_t getReceiveDataLength() = 0;

    virtual common::Error listening() = 0;

    virtual common::Error setIrqEventCallback(common::Callback cb,
                                              common::Argument arg) = 0;

    virtual common::radio::IrqEvent getIrqEvent() = 0;

    virtual common::SignalQuality getSignalQuality() = 0;
};
} // namespace radio
