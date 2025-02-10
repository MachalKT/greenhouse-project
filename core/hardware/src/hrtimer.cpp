#include "hrtimer.hpp"

#include "esp_timer.h"

namespace timer {
namespace hw {

uint8_t HrTimer::timerCount_{0};
std::queue<HrTimer::TimerNumber> HrTimer::availableTimerNumber_{};

common::Error HrTimer::init() {
  common::Error errorCode = setTimerProperties_();
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  esp_timer_create_args_t timerArgs = {};
  timerArgs.callback = timerCallback_;
  timerArgs.arg = this;
  timerArgs.name = name_.data();

  esp_err_t espErrorCode = esp_timer_create(
      &timerArgs, reinterpret_cast<esp_timer_handle_t*>(&handle_));
  if (espErrorCode != ESP_OK) {
    removeTimerProperties_();
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error HrTimer::deinit() {
  if (timerNumber_ == TimerNumber::TIMER_NOT_EXIST) {
    return common::Error::OK;
  }

  esp_err_t espErrorCode =
      esp_timer_delete(static_cast<esp_timer_handle_t>(handle_));
  if (espErrorCode != ESP_OK) {
    stop();
    esp_timer_delete(static_cast<esp_timer_handle_t>(handle_));
  }
  removeTimerProperties_();

  return common::Error::OK;
}

void HrTimer::setCallback(common::Callback cb, common::CallbackData data) {
  cb_ = cb;
  data_ = data;
}

common::Error HrTimer::startOnce(const common::Time timeUs) {
  esp_err_t espErrorCode =
      esp_timer_start_once(static_cast<esp_timer_handle_t>(handle_), timeUs);
  if (espErrorCode == ESP_ERR_INVALID_STATE) {
    espErrorCode =
        esp_timer_restart(static_cast<esp_timer_handle_t>(handle_), timeUs);
  }

  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error HrTimer::startPeriodic(const common::Time timeUs) {
  esp_err_t espErrorCode = esp_timer_start_periodic(
      static_cast<esp_timer_handle_t>(handle_), timeUs);
  if (espErrorCode == ESP_ERR_INVALID_STATE) {
    espErrorCode =
        esp_timer_restart(static_cast<esp_timer_handle_t>(handle_), timeUs);
  }

  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error HrTimer::stop() {
  esp_timer_stop(static_cast<esp_timer_handle_t>(handle_));
  return common::Error::OK;
}

void HrTimer::timerCallback_(void* arg) {
  if (not arg) {
    return;
  }

  HrTimer* timer = static_cast<HrTimer*>(arg);
  if (timer->cb_) {
    timer->cb_(timer->data_);
  }
}

common::Error HrTimer::setTimerProperties_() {
  if (timerNumber_ != TimerNumber::TIMER_NOT_EXIST) {
    return common::Error::INVALID_STATE;
  }

  ++timerCount_;
  if (not isPossibleCreateTimer_()) {
    --timerCount_;
    return common::Error::FAIL;
  }

  setTimerNumber_();
  name_ = std::string{"Timer"} + std::to_string(static_cast<int>(timerNumber_));

  return common::Error::OK;
}

void HrTimer::removeTimerProperties_() {
  availableTimerNumber_.push(timerNumber_);
  timerNumber_ = TimerNumber::TIMER_NOT_EXIST;
  --timerCount_;
  name_ = INVALID_NAME.data();
}

bool HrTimer::isPossibleCreateTimer_() {
  return timerCount_ > MAX_TIMER_COUNT ? false : true;
}

void HrTimer::setTimerNumber_() {
  TimerNumber newTimerNumber{TimerNumber::TIMER_NOT_EXIST};
  if (not availableTimerNumber_.empty()) {
    newTimerNumber = availableTimerNumber_.front();
    availableTimerNumber_.pop();
  } else {
    newTimerNumber = static_cast<TimerNumber>(timerCount_);
  }

  timerNumber_ = newTimerNumber;
}

} // namespace hw
} // namespace timer
