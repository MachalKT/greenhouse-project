#pragma once

#include "ticks.hpp"
#include "types.hpp"
#include <cstddef>
#include <functional>

namespace sw {

template <typename T> class Queue {
  public:
    using callback = std::function<void(T, common::Argument)>;

    using Handle = void*;

    Queue(size_t size);

    ~Queue();

    common::Error init();

    common::Error send(T& data);

    common::Error receive(T& data);

    void setCallback(callback cb, common::Argument arg);

    void yield();

  private:
    static constexpr ::sw::Ticks TICKS_TO_WAIT{0};
    Handle handle_;
    size_t size_;
    callback cb_;
    common::Argument arg_;
};

} // namespace sw