#pragma once

#include <cstdint>
#include <functional>

#include "types.hpp"

namespace timer {
using TimerHandle = void*;

class ITimer {
  public:
    virtual void setCallback(common::Callback cb, common::Argument arg) = 0;

    virtual common::Error startOnce(const common::Time timeUs) = 0;

    virtual common::Error startPeriodic(const common::Time timeUs) = 0;

    virtual common::Error stop() = 0;
};
} // namespace timer
