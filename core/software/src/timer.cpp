#include "timer.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"

namespace timer {
namespace sw {

common::Error Timer::init() {
  TimerHandle_t handle =
      xTimerCreate(NAME.data(), ::sw::msToTicks(DEFAULT_TIME_MS), pdFALSE, this,
                   [](TimerHandle_t arg) {
                     configASSERT(arg);
                     Timer* timer = static_cast<Timer*>(pvTimerGetTimerID(arg));
                     if (timer->cb_) {
                       timer->cb_(timer->arg_);
                     }
                   });

  if (handle == nullptr) {
    return common::Error::FAIL;
  }

  handle_ = static_cast<void*>(handle);
  return common::Error::OK;
}

common::Error Timer::deinit() {
  /** @note: if xTimerIsTimerActive == pdFALSE timer is dormant */
  if (xTimerIsTimerActive(static_cast<TimerHandle_t>(handle_)) != pdFALSE) {
    stop();
  }

  BaseType_t baseErrorCode =
      xTimerDelete(static_cast<TimerHandle_t>(handle_), TICKS_TO_WAIT);
  return baseErrorCode == pdPASS ? common::Error::OK : common::Error::FAIL;
}

void Timer::setCallback(common::Callback cb, common::Argument arg) {
  cb_ = cb;
  arg_ = arg;
}

common::Error Timer::startOnce(const common::Time timeUs) {
  if (not handle_) {
    return common::Error::INVALID_STATE;
  }

  common::Error errorCode = setPeriodAndReload_(timeUs, false);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return start_();
}

common::Error Timer::startPeriodic(const common::Time timeUs) {
  if (not handle_) {
    return common::Error::INVALID_STATE;
  }

  common::Error errorCode = setPeriodAndReload_(timeUs, true);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return start_();
}

common::Error Timer::stop() {
  if (not handle_) {
    return common::Error::INVALID_STATE;
  }

  BaseType_t baseErrorCode =
      xTimerStop(static_cast<TimerHandle_t>(handle_), TICKS_TO_WAIT);
  return baseErrorCode == pdPASS ? common::Error::OK : common::Error::FAIL;
}

common::Error Timer::setPeriodAndReload_(common::Time timeUs, bool isReload) {
  BaseType_t baseErrorCode =
      xTimerChangePeriod(static_cast<TimerHandle_t>(handle_),
                         ::sw::usToTicks(timeUs), TICKS_TO_WAIT);
  if (baseErrorCode != pdPASS) {
    return common::Error::FAIL;
  }

  BaseType_t reload{pdFALSE};
  if (isReload) {
    reload = pdTRUE;
  }

  vTimerSetReloadMode(static_cast<TimerHandle_t>(handle_), reload);
  return common::Error::OK;
}

common::Error Timer::start_() {
  BaseType_t baseErrorCode =
      xTimerStart(static_cast<TimerHandle_t>(handle_), TICKS_TO_WAIT);
  return baseErrorCode == pdPASS ? common::Error::OK : common::Error::FAIL;
}

} // namespace sw
} // namespace timer
