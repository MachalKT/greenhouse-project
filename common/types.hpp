#pragma once

#include <cstdint>
#include <limits>

namespace common {
using Time = uint32_t;
using Callback = void (*)(void*);
using Argument = void*;

enum class Error : uint8_t {
  OK,
  FAIL,
  INVALID_ARG,
  INVALID_STATE,
  NO_MEM,
  NOT_FOUND,
};

struct __attribute__((packed)) Telemetry {
    float temperatureC{0.0f};
    float humidityRh{0.0f};
};

struct SignalQuality {
    static constexpr int16_t RSSI_INVALID_VALUE{
        std::numeric_limits<int16_t>::max()};
    static constexpr float SNR_INVALID_VALUE{std::numeric_limits<float>::max()};
    int16_t rssi{RSSI_INVALID_VALUE};
    float snr{RSSI_INVALID_VALUE};
};

namespace radio {
enum class IrqEvent : uint8_t { UNKNOWN, RX_DONE, TX_DONE };
}

namespace event {
// Identifies the source of an event.
using Base = const char*;
// Pointer to event data.
using Data = void*;
// Event identification number.
using Id = int32_t;
// Pointer to a function handling Wi-Fi events.
using HandlerFunction = void (*)(common::Argument, Base, Id, Data);
} // namespace event
} // namespace common
