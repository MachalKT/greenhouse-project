#pragma once

#include "ticks.hpp"
#include "types.hpp"
#include <cstddef>
#include <functional>

namespace sw {

/**
 * @class IQueueSender
 * @brief Interface for sending data to a queue.
 *
 * @tparam T Type of data to be sent.
 */
template <typename T> class IQueueSender {
  public:
    virtual common::Error send(const T data) = 0;
};

/**
 * @class IQueueReceiver
 * @brief Interface for receiving data from a queue.
 *
 * @tparam T Type of data to be received.
 */
template <typename T> class IQueueReceiver {
  public:
    using Callback = std::function<void(T, common::Argument)>;

    virtual void setCallback(Callback cb, common::Argument arg) = 0;

    virtual void yield() = 0;
};

/**
 * @class Queue
 * @brief Class representing a queue for sending and receiving data.
 *
 * @tparam T Type of data to be sent and received.
 */
template <typename T>
class Queue : public IQueueSender<T>, public IQueueReceiver<T> {
  public:
    using Callback = IQueueReceiver<T>::Callback;

    /**
     * @brief Construct a new Queue object.
     *
     * @param size Size of the queue.
     */
    explicit Queue(const size_t size);

    /**
     * @brief Destroy the Queue object.
     */
    ~Queue();

    /**
     * @brief Initialize the queue.
     *
     * @return common::Error indicating success or failure.
     */
    common::Error init();

    /**
     * @brief Send data to the queue.
     *
     * @param data Data to be sent.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Queue is not initialized.
     */
    common::Error send(const T data) override;

    /**
     * @brief Set the callback function to be called when data is received.
     *
     * @param cb Callback function.
     * @param arg Argument to be passed to the callback function.
     */
    void setCallback(Callback cb, common::Argument arg);

    /**
     * @brief Yield the current thread until data is received.
     */
    void yield();

  private:
    using Handle = void*;

    /**
     * @brief Receive data from the queue.
     *
     * @param data Reference to the data to be received.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Queue is not initialized.
     */
    common::Error receive_(T& data);

    static constexpr ::sw::Ticks TICKS_TO_WAIT{0};
    size_t size_;
    Handle handle_{nullptr};
    Callback cb_{nullptr};
    common::Argument arg_{nullptr};
};

} // namespace sw

#include "impl/queue.tpp"
