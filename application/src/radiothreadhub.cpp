#include "radiothreadhub.hpp"
#include "defs.hpp"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include <cstring>
#include <string_view>

namespace {
static std::string_view TAG{"RADIO"};
}

namespace app {

RadioThreadHub::RadioThreadHub(Config config)
    : ThreadBase{{"RadioThread", STACK_DEPTH, PRIORITY, CORE_ID}},
      config_{config} {}

void RadioThreadHub::run_() {
  config_.timeoutTimer.setCallback(
      [](void* arg) {
        assert(arg);
        RadioThreadHub* radioThread = static_cast<RadioThreadHub*>(arg);
        radioThread->config_.queue.send(def::ui::LedEvent::RADIO_TIMEOUT);
        ESP_LOGI(TAG.data(), "Radio timeout");
      },
      this);

  config_.requestTimer.setCallback(
      [](void* arg) {
        assert(arg);
        std::array<uint8_t, sizeof(packet::radio::Type)> buffer{};
        packet::radio::Parser::parseRequestToBytes(
            packet::radio::Type::TELEMETRY_REQUEST, buffer.data(),
            buffer.size());

        RadioThreadHub* radioThread = static_cast<RadioThreadHub*>(arg);
        radioThread->config_.radio.send(buffer.data(), buffer.size());
      },
      this);

  common::Error errorCode = config_.requestTimer.startPeriodic(REQUEST_TIME_US);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Start periodic fail");
  }

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

void RadioThreadHub::processRadioIrqEvent_() {
  common::radio::IrqEvent event = config_.radio.getIrqEvent();
  if (event == common::radio::IrqEvent::RX_DONE) {
    config_.timeoutTimer.stop();
    processReceiveData_();

  } else if (event == common::radio::IrqEvent::TX_DONE) {
    config_.radio.listening();
    config_.timeoutTimer.startOnce(TIMEOUT_TIME_US);
  }
}

void RadioThreadHub::processReceiveData_() {
  std::array<uint8_t, MAX_READ_BUFFER> buffer{};
  common::Error errorCode = config_.radio.receive(buffer.data(), buffer.size());
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Cannot read data");
  }

  packet::radio::Type packetType =
      packet::radio::Parser::getType(buffer.data());
  handlePacketData_(packetType, buffer.data(), buffer.size());
}

void RadioThreadHub::handlePacketData_(const packet::radio::Type& packetType,
                                       const uint8_t* buffer,
                                       const size_t bufferLength) {
  switch (packetType) {
  case packet::radio::Type::OK:
    ESP_LOGI(TAG.data(), "Read: OK");
    break;
  case packet::radio::Type::NOT_OK:
    ESP_LOGI(TAG.data(), "Read: NOT_OK");
    break;
  case packet::radio::Type::TELEMETRY:
    ESP_LOGI(TAG.data(), "Read: TELEMETRY");
    receiveTelemetry_(buffer, bufferLength);
    break;
  default:
    ESP_LOGI(TAG.data(), "Read fail packet");
  }
}

void RadioThreadHub::receiveTelemetry_(const uint8_t* buffer,
                                       const size_t bufferLength) {
  packet::radio::Telemetry telemetryPacket{config_.telemetry};
  common::Error errorCode =
      telemetryPacket.parseFromBytes(buffer, bufferLength);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Parse telemetry fail");
  }

  config_.telemetry = telemetryPacket.getTelemetry();
  ESP_LOGI(TAG.data(), "Telemetry: temperature: %.2f [C], humidity: %.2f [RH]",
           config_.telemetry.temperatureC, config_.telemetry.humidityRh);
}

void RadioThreadHub::setRequestTimer_() {
  config_.requestTimer.setCallback(
      [](void* arg) {
        assert(arg);
        std::array<uint8_t, sizeof(packet::radio::Type)> buffer{};
        packet::radio::Parser::parseRequestToBytes(
            packet::radio::Type::TELEMETRY_REQUEST, buffer.data(),
            buffer.size());

        RadioThreadHub* radioThread = static_cast<RadioThreadHub*>(arg);
        radioThread->config_.radio.send(buffer.data(), buffer.size());
      },
      this);

  common::Error errorCode = config_.requestTimer.startPeriodic(REQUEST_TIME_US);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Start periodic fail");
  }
}

} // namespace app
