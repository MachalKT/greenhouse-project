#pragma once

#include "types.hpp"
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace app {
/**
 * @class AwsIotClient
 * @brief A client for interacting with AWS IoT Core.
 *
 * This class provides functionality to connect, publish, subscribe, and manage
 * communication with AWS IoT Core using MQTT. It supports setting callbacks for
 * disconnect and subscription events.
 */
class AwsIotClient {
  public:
    /**
     * @brief Callback type for handling disconnect events.
     */
    using disconnectCallback = std::function<void(common::Argument)>;

    /**
     * @brief Callback type for handling subscription messages.
     */
    using subscribeCallback = std::function<void(const char*, uint16_t, void*,
                                                 size_t, common::Argument)>;

    /**
     * @brief Quality of Service levels for MQTT messages.
     */
    enum class Qos : uint8_t { _0, _1 };

    /**
     * @brief Holds the TLS certificates required for secure communication.
     */
    struct Certificates {
        const uint8_t* rootCA;
        const uint8_t* cert;
        const uint8_t* privateKey;
    };

    /**
     * @brief Constructor for AwsIotClient.
     * @param clientId The unique client ID for the MQTT connection.
     */
    explicit AwsIotClient(const std::string_view& clientId);

    /**
     * @brief Destructor for AwsIotClient.
     */
    ~AwsIotClient() = default;

    /**
     * @brief Initializes the client with the host URL and certificates.
     *
     * @param hostUrl The URL of the AWS IoT Core endpoint.
     * @param certificates The TLS certificates for secure communication.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init(char* hostUrl, const Certificates certificates);

    /**
     * @brief Sets the callback for handling disconnect events.
     *
     * @param cb The disconnect callback function.
     * @param arg User-defined argument passed to the callback.
     */
    void setDisconnectCallback(disconnectCallback cb, common::Argument arg);

    /**
     * @brief Subscribes to a topic with a specified QoS level.
     *
     * @param topic The topic to subscribe to.
     * @param qos The Quality of Service level.
     * @param cb The callback function for handling received messages.
     * @param arg User-defined argument passed to the callback.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error subscribe(const std::string_view& topic, const Qos qos,
                            subscribeCallback cb, common::Argument arg);

    /**
     * @brief Establishes a connection to AWS IoT Core.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error connect();

    /**
     * @brief Disconnects from AWS IoT Core.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error disconnect();

    /**
     * @brief Publishes a message to a specified topic.
     *
     * @param topic The topic to publish to.
     * @param payload The message payload.
     * @param payloadSize The size of the payload.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error publish(const std::string_view& topic, char* payload,
                          size_t payloadSize);

    /**
     * @brief Processes incoming messages and maintains the connection.
     * This function should be called periodically to handle MQTT communication.
     */
    void yield();

  private:
    using Handler = void;
    using HandlerDeleter =
        void (*)(Handler*); ///< Deleter for the MQTT client handler.

    /**
     * @brief Stores callback data for a subscription.
     */
    struct SubscribeCallbackData {
        subscribeCallback cb;
        common::Argument arg;
    };

    /**
     * @brief Deletes the MQTT client handler.
     *
     * @param clientHandler Pointer to the MQTT client handler.
     */
    static void deleteHandler_(Handler* clientHandler);

    static constexpr uint32_t YIELD_TIMEOUT_MS{100};
    static constexpr uint32_t MQTT_COMMAND_TIMEOUT_MS{20'000};
    static constexpr uint32_t TLS_HANDSHAKE_TIMEOUT_MS{5000};
    static constexpr size_t PAYLOAD_SIZE{256};
    static constexpr uint16_t MQTT_KEEP_ALIVE_INTERVAL_S{10};

    std::map<std::string_view, SubscribeCallbackData> subscribeCallbacks_{};
    std::string_view clientId_;
    std::unique_ptr<Handler, HandlerDeleter> clientHandler_;
    disconnectCallback disconnectCb_{nullptr};
    common::Argument disconnectArg_{nullptr};
};
} // namespace app
