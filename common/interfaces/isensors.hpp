#pragma once

class ITemperatureSensor {
  public:
    virtual float getTemperatureC() = 0;
};

class IHumiditySensor {
  public:
    virtual float getHumidityRh() = 0;
};
