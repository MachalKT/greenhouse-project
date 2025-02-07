#pragma once

#include "interfaces/isensors.hpp"
#include "interfaces/itimer.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace app {
class TimedMeter {
  public:
    struct Config {
        timer::ITimer& measurementTimer;
        sensor::ITemperatureSensor& temperatureSensor;
        sensor::IHumiditySensor& humiditySensor;
    };

    /**
     * @brief Construct a new TimedMeter object.
     *
     * @param config Config.
     */
    explicit TimedMeter(Config config);

    /**
     * @brief Start periodic measurements.
     *
     * @param timeUs Period of time.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL Fail.
     */
    common::Error start(common::Time timeUs);

    /**
     * @brief Stop periodic measurements.
     *
     * @return
     *   - common::Error::OK Success.
     */
    common::Error stop();

    /**
     * @brief Get measurement Data.
     *
     * @return
     *   - common::MeasurementData Measurement Data.
     */
    common::MeasurementData getMeasurementData();

    /**
     * @brief Performs a measurement if a timer interrupt occurs.
     */
    void yield();

  private:
    void takeMeasurement_();

    Config config_;
    common::MeasurementData measurementValues_;
    bool isReadyToMeasure_{true};
};

} // namespace app
