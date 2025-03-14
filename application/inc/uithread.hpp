#pragma once

#include "button.hpp"
#include "defs.hpp"
#include "iled.hpp"
#include "queue.hpp"
#include "threadbase.hpp"
#include "timer.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace app {

/**
 * @class UiThread
 * @brief Class representing a thread for handling UI events.
 */
class UiThread final : public sw::ThreadBase {
  public:
    /**
     * @brief Configuration structure for UiThread.
     */
    struct Config {
        ui::Button& button;
        ui::ILed& led;
        sw::IQueueReceiver<def::ui::LedEvent>& queue;
        timer::ITimer& ledTimer;
    };

    /**
     * @brief Construct a new UiThread object.
     *
     * @param config Configuration for the UiThread.
     */
    explicit UiThread(Config config);

    /**
     * @brief Destroy the UiThread object.
     */
    ~UiThread() = default;

  private:
    /**
     * @brief Main function to run the thread.
     */
    void run_() override;

    /**
     * @brief Handle LED event.
     *
     * @param event LED event to handle.
     */
    void handleLedEvent_(def::ui::LedEvent& event);

    void setLedTimer_();

    static constexpr common::Time DISCONNECTED_LED_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, common::Time>(2))};
    static constexpr uint32_t STACK_DEPTH{2048};
    static constexpr int PRIORITY{5};
    static constexpr sw::ThreadBase::CoreId CORE_ID{sw::ThreadBase::CoreId::_0};
    Config config_;
    def::ui::LedEvent radioLedEvent_{def::ui::LedEvent::UNKNOWN};
    def::ui::LedEvent wifiLedEvent_{def::ui::LedEvent::UNKNOWN};
};

} // namespace app
