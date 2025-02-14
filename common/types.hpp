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

} // namespace common
