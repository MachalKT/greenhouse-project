#include "queue.hpp"
#include "freertos/idf_additions.h"
#include <vector>

namespace sw {
Queue::Queue(size_t size, size_t itemSize) : size_{size}, itemSize_{itemSize} {}

Queue::~Queue() {}

common::Error Queue::init() {
  handle_ = static_cast<Handle>(xQueueCreate(size_, itemSize_));
  if (not handle_) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Queue::send(const uint8_t* data, const size_t dataSize) {
  if (dataSize != itemSize_) {
    return common::Error::INVALID_ARG;
  }

  if (not handle_) {
    return common::Error::INVALID_STATE;
  }

  BaseType_t baseErrorCode =
      xQueueSend(static_cast<QueueHandle_t>(handle_), data, TICKS_TO_WAIT);
  if (baseErrorCode != pdPASS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

void Queue::setReceiveCallback(transport::receiveCb cb, common::Argument arg) {
  cb_ = cb;
  arg_ = arg;
}

void Queue::yield() {
  if (not handle_) {
    return;
  }

  std::vector<uint8_t> data(itemSize_);
  if (xQueueReceive(static_cast<QueueHandle_t>(handle_), data.data(),
                    TICKS_TO_WAIT) ==
      pdPASS) { // Timeout = 0 → tryb non-blocking
    cb_(data.data(), itemSize_, arg_);
  }
}

} // namespace sw