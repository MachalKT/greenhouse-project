#pragma once

#include "itimer.hpp"
#include "ticks.hpp"
#include "types.hpp"
#include <string_view>

namespace timer {
namespace sw {
class Timer final : public ITimer {
  public:
    Timer() = default;

    common::Error init();

    common::Error deinit();

    void setCallback(common::Callback cb, common::Argument arg) override;

    common::Error startOnce(const common::Time timeUs) override;

    common::Error startPeriodic(const common::Time timeUs) override;

    common::Error stop() override;

  private:
    common::Error setPeriodAndReload_(common::Time timeUs, bool isReload);

    common::Error start_();

    static constexpr std::string_view NAME{"Timer"};
    static constexpr common::Time DEFAULT_TIME_MS{10};
    static constexpr ::sw::Ticks TICKS_TO_WAIT{0};
    common::Callback cb_{nullptr};
    common::Argument arg_{nullptr};
    TimerHandle handle_{nullptr};
};

} // namespace sw
} // namespace timer
