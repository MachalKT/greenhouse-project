#pragma once

#include "defs.hpp"
#include "igpio.hpp"
#include "iradio.hpp"
#include "itimer.hpp"
#include "radiopacket.hpp"
#include "threadbase.hpp"
#include "utils.hpp"
#include <functional>
#include <optional>

namespace app {

/**
 * @class RadioThread
 * @brief Class representing a thread for handling radio communication.
 */
class RadioThread : public sw::ThreadBase {
  public:
    /**
     * @brief Configuration structure for RadioThread.
     */
    struct Config {
        radio::IRadio& radio;
        std::optional<std::reference_wrapper<timer::ITimer>> requestTimer;
        common::Telemetry& telemetry;
    };

    /**
     * @brief Construct a new RadioThread object.
     *
     * @param config Configuration for the RadioThread.
     */
    RadioThread(Config config);

  private:
    /**
     * @brief Main function to run the thread.
     */
    void run_() override;

    /**
     * @brief Process radio IRQ event.
     */
    void processRadioIrqEvent_();

    /**
     * @brief Process received data from the radio.
     */
    void processReceiveData_();

    /**
     * @brief Handle packet data based on the packet type.
     *
     * @param packetType Type of the packet.
     * @param buffer Pointer to the buffer containing the packet data.
     * @param bufferLength Length of the buffer.
     */
    void handlePacketData_(const packet::radio::Type& packetType,
                           const uint8_t* buffer, const size_t bufferLength);

    /**
     * @brief Send telemetry data.
     */
    void sendTelemetry_();

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

    static constexpr common::Time REQUEST_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, common::Time>(10))};
    static constexpr size_t MAX_READ_BUFFER{256};
    static constexpr uint32_t STACK_DEPTH{2880};
    static constexpr int PRIORITY{5};
    static constexpr sw::ThreadBase::CoreId CORE_ID{sw::ThreadBase::CoreId::_0};

    Config config_;
};

} // namespace app
