#pragma once

#include "ticks.hpp"
#include "types.hpp"
#include <cstddef>
#include <functional>

namespace sw {

template <typename T> class IQueueSender {
  public:
    virtual common::Error send(const T data) = 0;
};

template <typename T> class IQueueReceiver {
  public:
    using Callback = std::function<void(T, common::Argument)>;

    virtual void setCallback(Callback cb, common::Argument arg) = 0;

    virtual void yield() = 0;
};

template <typename T>
class Queue : public IQueueSender<T>, public IQueueReceiver<T> {
  public:
    using Callback = IQueueReceiver<T>::Callback;

    Queue(const size_t size);

    ~Queue();

    common::Error init();

    common::Error send(const T data) override;

    void setCallback(Callback cb, common::Argument arg);

    void yield();

  private:
    using Handle = void*;

    common::Error receive_(T& data);

    static constexpr ::sw::Ticks TICKS_TO_WAIT{0};
    Handle handle_;
    size_t size_;
    Callback cb_;
    common::Argument arg_;
};

} // namespace sw
