#pragma once

#include <cstdint>
#include <functional>

namespace common {
using Time = uint64_t;
using Callback = void(*)(void*);
using CallbackData = void*;

enum class Error : uint8_t{
    OK,
    FAIL,
    INVALID_ARG,
    INVALID_STATE,
};

struct MeasurementValues {
    float temperatureC{0.0f};
    float humidityRh{0.0f};
};

} //common
