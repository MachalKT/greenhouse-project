#pragma once

#include <queue>
#include <string>

#include "itimer.hpp"
#include "types.hpp"

namespace timer {
namespace hw {

/**
 * High Resolution Timer
 */
class HrTimer final : public ITimer {
  public:
    /**
     * @brief Initialize HrTimer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Timer is already initialized.
     */
    common::Error init();

    /**
     * @brief Deinitialize HrTimer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Timer is not initialized.
     */
    common::Error deinit();

    /**
     * @brief Set callback.
     * @note Callback is called in interrupt context.
     *
     * @param timerCallback Timer callback.
     * @param data Callback data.
     */
    void setCallback(common::Callback cb, common::Argument arg) override;

    /**
     * @brief Start timer once.
     * @note If timer is already started, restart timer.
     *
     * @param timeUs Time in microseconds.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error startOnce(const common::Time timeUs) override;

    /**
     * @brief Start timer periodically.
     * @note If timer is already started, restart timer.
     *
     * @param timeUs Time in microseconds.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error startPeriodic(const common::Time timeUs) override;

    /**
     * @brief Stop timer.
     *
     * @return
     *   - common::Error::OK: Success.
     */
    common::Error stop() override;

  private:
    /**
     * @brief Timer number.
     */
    enum class TimerNumber : uint8_t {
      TIMER_NOT_EXIST = 0,
      TIMER_1,
      TIMER_2,
      TIMER_3,
      TIMER_4
    };

    /**
     * @brief Timer callback.
     *
     * @param arg Timer callback data.
     */
    static void timerCallback_(void* arg);

    /**
     * @brief Set timer properties.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Timer is not exist.
     */
    common::Error setTimerProperties_();

    /**
     * @brief Remove timer properties.
     */
    void removeTimerProperties_();

    /**
     * @brief Is possible create timer.
     *
     * @return
     *   - true: Possible create timer.
     *   - false: Not possible create timer.
     */
    bool isPossibleCreateTimer_();

    /**
     * @brief Set timer number.
     */
    void setTimerNumber_();

    static constexpr uint8_t MAX_TIMER_COUNT{4};
    static constexpr std::string_view INVALID_NAME{"HrTimer not exist"};
    static uint8_t timerCount_;
    static std::queue<HrTimer::TimerNumber> availableTimerNumber_;
    TimerHandle handle_{nullptr};
    common::Callback cb_{nullptr};
    common::Argument arg_{nullptr};
    TimerNumber timerNumber_{TimerNumber::TIMER_NOT_EXIST};
    std::string name_{INVALID_NAME.data()};
};
} // namespace hw
} // namespace timer
