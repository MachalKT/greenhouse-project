#pragma once

#include <limits>

namespace sensor {
static constexpr float INVALID_VALUE{std::numeric_limits<float>::max()};

class ITemperatureSensor {
  public:
    virtual float getTemperatureC() = 0;
};

class IHumiditySensor {
  public:
    virtual float getHumidityRh() = 0;
};
} // namespace sensor
