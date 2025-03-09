#pragma once

#include "itransceiver.hpp"
#include "ticks.hpp"
#include "types.hpp"
#include <cstddef>
#include <functional>

namespace sw {

class Queue final : public transport::ITransceiver {
  public:
    using Handle = void*;

    Queue(size_t size, size_t itemSize);

    ~Queue();

    common::Error init();

    common::Error send(const uint8_t* data, const size_t dataSize) override;

    void setReceiveCallback(transport::receiveCb cb,
                            common::Argument arg) override;

    void yield() override;

  private:
    static constexpr ::sw::Ticks TICKS_TO_WAIT{0};
    Handle handle_;
    size_t size_;
    size_t itemSize_;
    transport::receiveCb cb_;
    common::Argument arg_;
};

} // namespace sw