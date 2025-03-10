#include "button.hpp"
#include "delay.hpp"
#include "esp_log.h"
#include "gpio.hpp"
#include "nvsstore.hpp"
#include "queue.hpp"
#include "radiothread.hpp"
#include "rfm95.hpp"
#include "spi.hpp"
#include "timer.hpp"
#include "uithread.hpp"
#include "wificontroller.hpp"
#include "ws2812b.hpp"
#include <string_view>

namespace {
static constexpr std::string_view TAG{"HUB"};
} // namespace

extern "C" {
void app_main(void) {
  common::Error errorCode{common::Error::OK};

  errorCode = storage::hw::NvsStore::init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "NVS init fail");
  }

  hw::Gpio mosi{23};
  hw::Gpio miso{19};
  hw::Gpio sck{18};
  hw::Spi spi{{miso, mosi, sck, hw::Spi::Host::VSPI}};
  errorCode = spi.init(hw::Spi::DmaChannel::CHANNEL_1);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "spi init fail");
  }

  hw::Gpio rfm95ChipSelect{5};
  hw::SpiDeviceHandle spiRfm95Handle =
      spi.addDevice(rfm95ChipSelect, radio::Rfm95::SPI_CLOCK_SPEED_HZ);

  hw::Gpio rst{4};
  hw::Gpio dio0{17};
  radio::Rfm95 rfm95{{rst, dio0, spi, spiRfm95Handle}};
  errorCode = rfm95.init(radio::Rfm95::Modulation::LORA);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "radio init fail");
  }

  radio::Rfm95::ModemSettings settings;
  settings.frequencyHz = 868'000'000;
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
    ESP_LOGE(TAG.data(), "radio set all settings fail");
  }

  storage::hw::NvsStore storage{"storage"};

  timer::sw::Timer wifiReconnectTimer;
  errorCode = wifiReconnectTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "wifiReconnectTimer init fail");
  }

  timer::sw::Timer radiorequestTimer;
  errorCode = radiorequestTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "RadioTimer init fail");
  }

  hw::Gpio redPin{12};
  hw::Gpio greenPin{27};
  hw::Gpio bluePin{26};
  ui::Ws2812b led{{redPin, greenPin, bluePin}};
  errorCode = led.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Led init fail");
  }

  hw::Gpio buttonPin{34};
  ui::Button button{buttonPin};
  errorCode = button.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Button init fail");
  }

  sw::Queue<def::ui::LedEvent> queueLedEvent{5};
  errorCode = queueLedEvent.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "QueueLedEvent init fail");
  }

  app::UiThread uiThread{{button, led, queueLedEvent}};
  errorCode = uiThread.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "UiThread start fail");
  }

  common::Telemetry telemetry{};
  app::RadioThread radioThread{{rfm95, radiorequestTimer, telemetry}};
  radioThread.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "RadioThread start fail");
  }

  app::WifiController wifiController{{wifiReconnectTimer, storage}};
  errorCode = wifiController.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "WifiController init fail");
  }

  errorCode = wifiController.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "WifiController start fail");
  }

  while (1) {
    button.yield();
    sw::delayMs(10);
  }
}
}
