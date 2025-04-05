#include "eventgroup.hpp"
#include "freertos/idf_additions.h"
#include "ticks.hpp"

namespace sw {
EventGroup::~EventGroup() {
  if (handler_ != nullptr) {
    vEventGroupDelete(static_cast<EventGroupHandle_t>(handler_));
    handler_ = nullptr;
  }
}

common::Error EventGroup::init() {
  handler_ = static_cast<Handler>(xEventGroupCreate());
  if (handler_ == nullptr) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error EventGroup::set(Bits bits) {
  if (handler_ == nullptr) {
    return common::Error::INVALID_STATE;
  }

  if (bits == 0) {
    return common::Error::INVALID_ARG;
  }

  Bits expectedAllBits = allBits_ | bits;
  allBits_ = static_cast<Bits>(
      xEventGroupSetBits(static_cast<EventGroupHandle_t>(handler_), bits));
  if (allBits_ != expectedAllBits) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error EventGroup::clear(Bits bits) {
  if (handler_ == nullptr) {
    return common::Error::INVALID_STATE;
  }

  if (bits == 0) {
    return common::Error::INVALID_ARG;
  }

  Bits expectedAllBits = allBits_ & (~bits);
  allBits_ = static_cast<Bits>(
      xEventGroupClearBits(static_cast<EventGroupHandle_t>(handler_), bits));
  if (allBits_ != expectedAllBits) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error EventGroup::wait(Bits bits, common::Time timeoutMs) {
  if (handler_ == nullptr) {
    return common::Error::INVALID_STATE;
  }

  if (bits == 0) {
    return common::Error::INVALID_ARG;
  }

  Bits expectedAllBits = allBits_ | bits;
  allBits_ = static_cast<Bits>(
      xEventGroupWaitBits(static_cast<EventGroupHandle_t>(handler_), bits,
                          pdFALSE, pdTRUE, msToTicks(timeoutMs)));
  if (allBits_ != expectedAllBits) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

EventGroup::Bits EventGroup::getBits() const { return allBits_; }

} // namespace sw
