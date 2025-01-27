#pragma once 

#include <string>
#include <queue>
#include "interfaces/itimer.hpp"
#include "types.hpp"
#include "esp_timer.h"

namespace timer {
namespace hw {
using TimerHandle = esp_timer_handle_t;

class HrTimer final : public ITimer {
  public:
    common::Error init();

    common::Error deinit();

    void setCallback(timer::TimerCallback timerCallback, timer::TimerData data) override;

    common::Error startOnce(const uint32_t timeUs) override;

    common::Error startPeriodic(const uint32_t timeUs) override;

    common::Error stop() override;
  
  private:
    enum class TimerNumber {
        TIMER_NOT_EXIST = 0,
        TIMER_1,
        TIMER_2,
        TIMER_3,
        TIMER_4
    };
    
    static void timerCallback(void* arg);

    bool isPossibleCreateTimer();

    void setTimerNumber();

    static constexpr uint8_t MAX_TIMER_COUNT{4};
    static uint8_t timerCount_;
    static std::queue<HrTimer::TimerNumber> availableTimerNumber_;
    TimerHandle handle_{nullptr};
    TimerCallback cb_{nullptr};
    TimerData data_{nullptr};
    TimerNumber timerNumber_{TimerNumber::TIMER_NOT_EXIST};
    std::string name_{""};
};
} //hw
} //timer
