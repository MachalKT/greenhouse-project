#pragma once

#include <cstdint>
#include <functional>
#include "types.hpp"

namespace timer {
using TimerData = void*;
using TimerCallback = std::function<void(void*)>;

class ITimer {
  public:
    virtual void setCallback(TimerCallback timerCallback, TimerData arg) = 0;

    virtual common::Error startOnce(const common::Time timeUs) = 0;

    virtual common::Error startPeriodic(const common::Time timeUs) = 0;

    virtual common::Error stop() = 0;
};
} //timer
