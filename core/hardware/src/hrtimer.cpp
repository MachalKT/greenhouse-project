#include "hrtimer.hpp"
#include "esp_log.h"

namespace timer{
namespace hw {

uint8_t HrTimer::timerCount_{0};
std::queue<HrTimer::TimerNumber> HrTimer::availableTimerNumber_{};

common::Error HrTimer::init() {
    common::Error errorCode = setTimerProperties();
    if(errorCode != common::Error::OK) {
        return errorCode;
    }
    
    esp_timer_create_args_t timerArgs = {};
    timerArgs.callback = timerCallback;
    timerArgs.arg = this;
    timerArgs.name = name_.data();

    esp_err_t espErrorCode = esp_timer_create(&timerArgs, &handle_);
    if(espErrorCode != ESP_OK) {
        removeTimerProperties();
        return common::Error::FAIL;
    }

    return common::Error::OK;
}

common::Error HrTimer::deinit() {
    if(timerNumber_ == TimerNumber::TIMER_NOT_EXIST) {
        return common::Error::OK;
    }
    
    esp_err_t errorCode = esp_timer_delete(handle_);
    if(errorCode != ESP_OK) {
        stop();
        esp_timer_delete(handle_);
    }
    removeTimerProperties();

    return common::Error::OK;
}

void HrTimer::setCallback(TimerCallback cb, TimerData data) {
    cb_ = cb;
    data_ = data; 
}

common::Error HrTimer::startOnce(const common::Time timeUs) {
    esp_err_t errorCode =  esp_timer_start_once(handle_, timeUs);
    if(errorCode == ESP_ERR_INVALID_STATE) {
        errorCode = esp_timer_restart(handle_, timeUs);
    }

    if(errorCode != ESP_OK) {
        return common::Error::FAIL;
    }

    return common::Error::OK;
}

common::Error HrTimer::startPeriodic(const common::Time timeUs) {
    esp_err_t errorCode =  esp_timer_start_periodic(handle_, timeUs);
    if(errorCode == ESP_ERR_INVALID_STATE) {
        errorCode = esp_timer_restart(handle_, timeUs);
    }

    if(errorCode != ESP_OK) {
        return common::Error::FAIL;
    }

    return common::Error::OK;
}

common::Error HrTimer::stop() {
    esp_timer_stop(handle_);
    return common::Error::OK;
}

void HrTimer::timerCallback(void* arg) {
    if(not arg) {
        return;
    }

    HrTimer* timer = static_cast<HrTimer*>(arg);
    timer->cb_(timer->data_);
}

common::Error HrTimer::setTimerProperties() {
    if(timerNumber_ != TimerNumber::TIMER_NOT_EXIST) {
        return common::Error::INVALID_STATE;
    }

    ++timerCount_;
    if(not isPossibleCreateTimer()) {
        --timerCount_;
        return common::Error::FAIL;
    }

    setTimerNumber();
    name_ = std::string{"Timer"} + std::to_string(static_cast<int>(timerNumber_));
    
    return common::Error::OK;
}

void HrTimer::removeTimerProperties() {
    availableTimerNumber_.push(timerNumber_);
    timerNumber_ = TimerNumber::TIMER_NOT_EXIST;
    --timerCount_;
    name_ = INVALID_NAME.data();
}

bool HrTimer::isPossibleCreateTimer() {
    return timerCount_ > MAX_TIMER_COUNT ? false : true;
}

void HrTimer::setTimerNumber() {
    TimerNumber newTimerNumber{TimerNumber::TIMER_NOT_EXIST};
    if(not availableTimerNumber_.empty()) {
        newTimerNumber = availableTimerNumber_.front();
        availableTimerNumber_.pop();
    }
    else {
        newTimerNumber = static_cast<TimerNumber>(timerCount_);
    }

    timerNumber_ = newTimerNumber;
}

} //hw
} //timer
