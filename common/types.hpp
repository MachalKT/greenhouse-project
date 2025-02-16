#pragma once

#include <cstdint>

namespace common {
using Time = uint64_t;
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

struct MeasurementData {
    float temperatureC{0.0f};
    float humidityRh{0.0f};
};

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
