#pragma once

#include "defs.hpp"
#include "iradio.hpp"
#include "itimer.hpp"
#include "queue.hpp"
#include "radiopacket.hpp"
#include "threadbase.hpp"
#include "utils.hpp"

namespace app {

class RadioThreadHub final : public sw::ThreadBase {
  public:
    struct Config {
        radio::IRadio& radio;
        timer::ITimer& requestTimer;
        timer::ITimer& timeoutTimer;
        sw::IQueueSender<def::ui::LedEvent>& ledEventQueue;
        sw::IQueueSender<common::Telemetry>& telemetryQueue;
    };

    explicit RadioThreadHub(Config config);

    ~RadioThreadHub() = default;

  private:
    void run_() override;

    void processRadioIrqEvent_();

    void processReceiveData_();

    void handlePacketData_(const packet::radio::Type& packetType,
                           const uint8_t* buffer, const size_t bufferLength);
    /**
     * @brief Receive telemetry data from the buffer.
     *
     * @param buffer Pointer to the buffer containing the telemetry data.
     * @param bufferLength Length of the buffer.
     */
    void receiveTelemetry_(const uint8_t* buffer, const size_t bufferLength);

    /**
     * @brief Set the request timer.
     */
    void setRequestTimer_();

    /**
     * @brief Set the timeout timer.
     */
    void setTimeoutTimer_();

    static constexpr common::Time REQUEST_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, common::Time>(10))};
    static constexpr common::Time TIMEOUT_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, common::Time>(5))};
    static constexpr size_t MAX_READ_BUFFER{256};
    static constexpr uint32_t STACK_DEPTH{2880};
    static constexpr int PRIORITY{5};
    static constexpr sw::ThreadBase::CoreId CORE_ID{sw::ThreadBase::CoreId::_0};
    Config config_;
};
} // namespace app
