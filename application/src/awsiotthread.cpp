#include "awsiotthread.hpp"
#include "awspacket.hpp"
#include "delay.hpp"
#include "esp_log.h"

namespace {
static std::string_view TAG{"AWS_IOT"};
}

namespace app {
AwsIotThread::AwsIotThread(Config config)
    : ThreadBase{{"AwsIotThread", STACK_DEPTH, PRIORITY, CORE_ID}},
      config_{config} {}

void AwsIotThread::run_() {
  config_.reconnectTimer.setCallback(
      [](void* arg) {
        assert(arg);
        AwsIotThread* thread = static_cast<AwsIotThread*>(arg);
        thread->isConnectTriggered_ = true;
      },
      this);

  config_.awsIotClient.setDisconnectCallback(
      [](common::Argument arg) {
        assert(arg);
        AwsIotThread* thread = static_cast<AwsIotThread*>(arg);
        thread->handleDisconnect_();
      },
      this);

  config_.telemetryQueue.setCallback(
      [](common::Telemetry telemetry, common::Argument arg) {
        assert(arg);
        auto* awsThread = static_cast<AwsIotThread*>(arg);
        common::Error errorCode = awsThread->publishTelemetry_(telemetry);
        if (errorCode != common::Error::OK) {
          ESP_LOGE(TAG.data(), "Failed to publish telemetry");
        }
      },
      this);

  while (config_.connectionEventGroup.wait(def::net::WIFI_CONNECTED_BIT,
                                           CONNECTED_WAIT_TIMEOUT_MS) !=
         common::Error::OK) {
    ESP_LOGW(TAG.data(), "Waiting for wifi connected");
  }

  isConnectTriggered_ = true;

  while (1) {
    yield_();
    config_.telemetryQueue.yield();
    config_.awsIotClient.yield();
    sw::delayMs(10);
  }
}

void AwsIotThread::yield_() {
  if (not config_.connectionEventGroup.isBitsSet(
          def::net::WIFI_CONNECTED_BIT)) {
    return;
  }

  if (not config_.connectionEventGroup.isBitsSet(def::net::AWS_CONNECTED_BIT)) {
    if (isConnectTriggered_) {
      isConnectTriggered_ = false;
      handleConnect_();
    }
    return;
  }
}

void AwsIotThread::handleConnect_() {
  common::Error errorCode = config_.awsIotClient.connect();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to connect to AWS IoT");
    handleReconnect_();
    return;
  }

  ESP_LOGI(TAG.data(), "Connected to AWS IoT");
  config_.connectionEventGroup.set(def::net::AWS_CONNECTED_BIT);
  config_.ledEventQueue.send(def::ui::LedEvent::AWS_CONNECTED);
  setSubscriptions_();
}

void AwsIotThread::handleReconnect_() {
  ++connectCounter;
  if (connectCounter >= MAX_CONNECT_ATTEMPTS) {
    connectCounter = 0;
    config_.reconnectTimer.startOnce(RECONNECT_AFTER_MAX_ATTEMPT_TIME_US);
    return;
  }

  const auto reconnectTimeUs =
      RECONNECT_TIME_US * static_cast<common::Time>(connectCounter);
  config_.reconnectTimer.startOnce(reconnectTimeUs);
}

void AwsIotThread::handleDisconnect_() {
  config_.connectionEventGroup.clear(def::net::AWS_CONNECTED_BIT);
  config_.ledEventQueue.send(def::ui::LedEvent::AWS_DISCONNECTED);
  isConnectTriggered_ = true;
  ESP_LOGI(TAG.data(), "Disconnected from AWS IoT");
}

void AwsIotThread::setSubscriptions_() {
  common::Error errorCode = config_.awsIotClient.subscribe(
      TELEMETRY_TOPIC, AwsIotClient::Qos::_1,
      [](const char* topic, uint16_t packetId, void* payload,
         size_t payloadLength, common::Argument arg) {
        ESP_LOGI(TAG.data(), "Received message on topic: %s", topic);
      },
      nullptr);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to subscribe to topic: %s",
             TELEMETRY_TOPIC.data());
  }
}

common::Error AwsIotThread::publishTelemetry_(common::Telemetry telemetry) {
  std::array<char, packet::aws::BUFFER_SIZE> buffer{};
  packet::aws::Telemetry telemetryPacket(telemetry);
  common::Error errorCode =
      telemetryPacket.parseToJsonString(buffer.data(), buffer.size());
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to parse telemetry to JSON");
    return errorCode;
  }

  return config_.awsIotClient.publish(TELEMETRY_TOPIC, buffer.data(),
                                      buffer.size());
}

} // namespace app