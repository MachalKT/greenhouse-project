#pragma once

#include "awsiotclient.hpp"
#include "defs.hpp"
#include "eventgroup.hpp"
#include "itimer.hpp"
#include "queue.hpp"
#include "threadbase.hpp"
#include "utils.hpp"

namespace app {
/**
 * @class AwsIotThread
 * @brief A thread class for managing AWS IoT communication.
 *
 * The `AwsIotThread` class is responsible for handling AWS IoT-related
 * operations in a dedicated thread.
 */
class AwsIotThread : public sw::ThreadBase {
  public:
    /**
     * @brief Configuration structure for initializing the `AwsIotThread`.
     */
    struct Config {
        AwsIotClient& awsIotClient;
        sw::EventGroup& connectionEventGroup;
        sw::IQueueReceiver<common::Telemetry>& telemetryQueue;
        sw::IQueueSender<def::ui::LedEvent>& ledEventQueue;
        timer::ITimer& reconnectTimer;
    };

    /**
     * @brief Constructs for AwsIotThread
     *
     * @param config The configuration structure containing required
     * dependencies.
     */
    explicit AwsIotThread(Config config);

    /**
     * @brief Default destructor for the `AwsIotThread` class.
     */
    ~AwsIotThread() = default;

  private:
    /**
     * @brief The main thread execution function.
     */
    void run_() override;

    /**
     * @brief Yields the thread to allow other operations to execute.
     */
    void yield_();

    /**
     *  @brief Handles the logic for managing disconnection events.
     */
    void handleDisconnect_();

    /**
     * @brief Handles the logic for managing connection events.
     */
    void handleConnect_();

    /**
     * @brief Handles the logic for managing reconnection attempts.
     */
    void handleReconnect_();

    /**
     * @brief Sets up subscriptions to AWS IoT topics.
     */
    void setSubscriptions_();

    /**
     * @brief Publishes telemetry data to the AWS IoT Core.
     *
     * @param telemetry The telemetry data to be published.
     *
     * @return
     *   - common::Error::OK: Telemetry was successfully published.
     *   - common::Error::FAIL: Failed to publish telemetry.
     */
    common::Error publishTelemetry_(common::Telemetry telemetry);

    static constexpr std::string_view TELEMETRY_TOPIC{"controller/telemetry"};
    static constexpr common::Time CONNECTED_WAIT_TIMEOUT_MS{
        common::utils::sToMs<common::Time, common::Time>(60)};
    static constexpr common::Time RECONNECT_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, common::Time>(10))};
    static constexpr common::Time RECONNECT_AFTER_MAX_ATTEMPT_TIME_US{
        common::utils::msToUs<common::Time, common::Time>(
            common::utils::sToMs<common::Time, common::Time>(
                common::utils::minToS<common::Time, common::Time>(10)))};
    static constexpr uint32_t STACK_DEPTH{4096};
    static constexpr int PRIORITY{4};
    static constexpr sw::ThreadBase::CoreId CORE_ID{sw::ThreadBase::CoreId::_0};
    static constexpr uint8_t MAX_CONNECT_ATTEMPTS{5};
    Config config_;
    uint8_t connectCounter{0};
    bool isConnectTriggered_{false};
};

} // namespace app
