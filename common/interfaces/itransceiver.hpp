#pragma once

#include "types.hpp"
#include <cstddef>
#include <functional>

namespace transport {
using receiveCb = std::function<void(const uint8_t* data, const size_t dataSize,
                                     common::Argument arg)>;

class ITransceiver {
  public:
    virtual common::Error send(const uint8_t* data, const size_t dataSize) = 0;

    virtual void setReceiveCallback(receiveCb cb, common::Argument arg) = 0;

    virtual void yield() = 0;
};
}; // namespace transport