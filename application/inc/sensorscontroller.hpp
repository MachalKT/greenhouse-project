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

    explicit SensorsController(Config config);

    common::Error start(common::Time timeUs);

    common::Error stop();

    common::MeasurementValues getMeasurementValues();

    void yield();

  private:
    void takeMeasurement();

    Config config_;
    common::MeasurementValues measurementValues_;
    bool isReadyToMeasure_{true};
};

} //app
