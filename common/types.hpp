#pragma once

namespace common {
enum class Error {
    OK,
    FAIL
};

struct MeasurementValues {
    float temperatureC{0.0f};
    float humidityRh{0.0f};
};

} //common
