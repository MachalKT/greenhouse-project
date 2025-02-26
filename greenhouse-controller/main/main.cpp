#include "adc.hpp"
#include "esp_log.h"
#include "gpio.hpp"
#include "hrtimer.hpp"
#include "i2c.hpp"
#include "rfm95.hpp"
#include "sht40.hpp"
#include "spi.hpp"
#include "timedmeter.hpp"
#include "timer.hpp"
#include "types.hpp"
#include "utils.hpp"

namespace {
const char* TAG = "MAIN";
static constexpr common::Time MEASUREMENT_TIME_US{
    common::utils::msToUs<common::Time, common::Time>(
        common::utils::sToMs<common::Time, common::Time>(1))};
static constexpr uint64_t RADIO_FREQUENCY_HZ{868'000'000};

} // namespace

extern "C" {
void app_main(void) {
  common::Error errorCode{common::Error::OK};

  hw::Gpio miso{19};
  hw::Gpio mosi{23};
  hw::Gpio sck{18};
  hw::Spi spi{{miso, mosi, sck, hw::spi::Host::VSPI}};

  errorCode = spi.init(hw::spi::DmaChannel::CHANNEL_1);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "spi init fail");
  }

  hw::I2c i2c;
  errorCode = i2c.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "i2c init fail");
  }

  hw::Gpio batteryGpio{32};
  hw::Adc adc{batteryGpio};
  errorCode =
      adc.init(hw::adc::BitWidth::BITWIDTH_12, hw::adc::Attenuation::DB_11);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "adc init fail");
  }

  sensor::Sht40 sht40{i2c};
  errorCode = sht40.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "sht40 init fail");
  }

  hw::Gpio rfm95ChipSelect{5};
  hw::SpiDeviceHandle spiRfm95Handle =
      spi.addDevice(rfm95ChipSelect, static_cast<int>(hw::Spi::CLOCK_SPEED_HZ));
  if (spiRfm95Handle == nullptr) {
    ESP_LOGE(TAG, "spi add device fail");
  }

  hw::Gpio rst{4};
  hw::Gpio dio0{17};
  radio::Rfm95 rfm95{{rst, dio0, spi, spiRfm95Handle}};
  errorCode = rfm95.init(radio::Rfm95::Modulation::LORA);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "radio init fail");
  }

  radio::Rfm95::ModemSettings settings;
  settings.frequencyHz = RADIO_FREQUENCY_HZ;
  settings.lnaBoostHfEnable = true;
  settings.gain = radio::Rfm95::Gain::G1;
  settings.bandwidth = radio::Rfm95::Bandwidth::BW_125000;
  settings.spreadingFactor = radio::Rfm95::SF::SF_12;
  settings.syncWord = 18;
  settings.preambleLength = 8;
  settings.paPin = radio::Rfm95::PaPin::BOOST;
  settings.power = 20;

  errorCode = rfm95.setAllSettings(settings);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "radio set all settings fail");
  }

  timer::hw::HrTimer measurementTimer;
  measurementTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG, "Measurement timer init fail");
  }

  app::TimedMeter timedMeter{{measurementTimer, sht40, sht40}};
  timedMeter.start(MEASUREMENT_TIME_US);

  while (1) {
    timedMeter.yield();
    sw::delayMs(10);
  }
}
}
