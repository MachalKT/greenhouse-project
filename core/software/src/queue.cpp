#include "queue.hpp"
#include "defs.hpp"
#include "freertos/idf_additions.h"

namespace sw {
template <typename T> Queue<T>::Queue(size_t size) : size_{size} {}

template <typename T> Queue<T>::~Queue() {
  if (handle_) {
    vQueueDelete(static_cast<QueueHandle_t>(handle_));
  }
}

template <typename T> common::Error Queue<T>::init() {
  handle_ = static_cast<Handle>(xQueueCreate(size_, sizeof(T)));
  if (not handle_) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

template <typename T> common::Error Queue<T>::send(T data) {
  if (not handle_) {
    return common::Error::INVALID_STATE;
  }

  BaseType_t baseErrorCode =
      xQueueSend(static_cast<QueueHandle_t>(handle_), &data, TICKS_TO_WAIT);
  if (baseErrorCode != pdPASS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

template <typename T>
void Queue<T>::setCallback(Callback cb, common::Argument arg) {
  cb_ = cb;
  arg_ = arg;
}

template <typename T> void Queue<T>::yield() {
  T data;
  if (receive_(data) == common::Error::OK) { // Timeout = 0 → tryb non-blocking
    cb_(data, arg_);
  }
}

template <typename T> common::Error Queue<T>::receive_(T& data) {
  if (not handle_) {
    return common::Error::INVALID_STATE;
  }

  BaseType_t baseErrorCode =
      xQueueReceive(static_cast<QueueHandle_t>(handle_), &data, TICKS_TO_WAIT);
  if (baseErrorCode != pdPASS) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

template class Queue<def::ui::LedEvent>;
} // namespace sw
