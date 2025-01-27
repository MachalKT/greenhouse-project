#pragma once

#include "interfaces/isensors.hpp"
#include "interfaces/itimer.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace app {

class SensorsController {
  public:
    struct Config {
        timer::ITimer& measurementTimer;
        ITemperatureSensor& temperatureSensor;
        IHumiditySensor& humiditySensor;
    };

    SensorsController(Config config);

    common::Error start(uint32_t timeUs);

    common::Error stop();

    common::MeasurementValues getMeasurementValues();

    void yield();

  private:
    void takeMeasurement();

    Config config_;
    common::MeasurementValues measurementValues_;
    bool isInterrupt_{false};
};

} //app
