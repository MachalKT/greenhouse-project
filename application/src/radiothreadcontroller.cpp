#include "radiothreadcontroller.hpp"
#include "esp_log.h"
#include <array>
#include <string_view>

namespace {
static constexpr std::string_view TAG{"Radio"};
}

namespace app {
RadioThreadController::RadioThreadController(Config config)
    : ThreadBase{{"RadioThread", STACK_DEPTH, PRIORITY, CORE_ID}},
      config_{config} {}

void RadioThreadController::run_() {
  config_.radio.setIrqEventCallback(
      [](void* arg) {
        assert(arg);
        resumeFromISR_(static_cast<ThreadHandle>(arg));
      },
      handle_);

  config_.radio.listening();
  while (1) {
    suspend_();
    processRadioIrqEvent_();
  }
}

void RadioThreadController::processRadioIrqEvent_() {
  common::radio::IrqEvent event = config_.radio.getIrqEvent();
  if (event == common::radio::IrqEvent::RX_DONE) {
    processReceiveData_();

  } else if (event == common::radio::IrqEvent::TX_DONE) {
    config_.radio.listening();
  }
}

void RadioThreadController::processReceiveData_() {
  std::array<uint8_t, MAX_READ_BUFFER> buffer{};
  common::Error errorCode = config_.radio.receive(buffer.data(), buffer.size());
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Cannot read data");
  }

  packet::radio::Type packetType =
      packet::radio::Parser::getType(buffer.data());
  handlePacketData_(packetType, buffer.data(), buffer.size());
}

void RadioThreadController::handlePacketData_(
    const packet::radio::Type& packetType, const uint8_t* buffer,
    const size_t bufferLength) {
  switch (packetType) {
  case packet::radio::Type::OK:
    ESP_LOGI(TAG.data(), "Read: OK");
    break;
  case packet::radio::Type::NOT_OK:
    ESP_LOGI(TAG.data(), "Read: NOT_OK");
    break;
  case packet::radio::Type::TELEMETRY_REQUEST:
    ESP_LOGI(TAG.data(), "Read: TELEMETRY_REQUEST");
    sendTelemetry_();
    break;
  default:
    ESP_LOGI(TAG.data(), "Read fail packet");
  }
}

void RadioThreadController::sendTelemetry_() {
  packet::radio::Telemetry telemetryPacket{config_.telemetry};
  std::array<uint8_t, sizeof(packet::radio::Telemetry)> buffer{};

  common::Error errorCode =
      telemetryPacket.parseToBytes(buffer.data(), buffer.size());
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Parse telemetry fail");
  }

  ESP_LOGI(TAG.data(), "Send telemetry");
  errorCode = config_.radio.send(buffer.data(), buffer.size());
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Send telemetry fail");
  }
}

} // namespace app
