#pragma once

namespace common {
using Time = uint64_t;

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
