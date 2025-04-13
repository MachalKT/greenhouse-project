#include "awsiotclient.hpp"
#include "awsiotthread.hpp"
#include "button.hpp"
#include "delay.hpp"
#include "esp_log.h"
#include "eventgroup.hpp"
#include "gpio.hpp"
#include "nvsstore.hpp"
#include "queue.hpp"
#include "radiothreadhub.hpp"
#include "rfm95.hpp"
#include "spi.hpp"
#include "timer.hpp"
#include "uithread.hpp"
#include "utils.hpp"
#include "wificontroller.hpp"
#include "ws2812b.hpp"
#include <cstring>
#include <string_view>

namespace {
extern const uint8_t
    binaryAmazonRootCA1[] asm("_binary_amazonRootCA1_pem_start");
extern const uint8_t
    binaryCertificate[] asm("_binary_certificate_pem_crt_start");
extern const uint8_t binaryPrivateKey[] asm("_binary_private_pem_key_start");
static constexpr std::string_view TAG{"HUB"};
} // namespace

extern "C" {
void app_main(void) {
  common::Error errorCode{common::Error::OK};

  errorCode = storage::hw::NvsStore::init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init nvs");
  }

  hw::Gpio mosi{23};
  hw::Gpio miso{19};
  hw::Gpio sck{18};
  hw::Spi spi{{miso, mosi, sck, hw::Spi::Host::VSPI}};
  errorCode = spi.init(hw::Spi::DmaChannel::CHANNEL_1);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init spi");
  }

  hw::Gpio rfm95ChipSelect{5};
  hw::SpiDeviceHandle spiRfm95Handle =
      spi.addDevice(rfm95ChipSelect, radio::Rfm95::SPI_CLOCK_SPEED_HZ);
  if (spiRfm95Handle == nullptr) {
    ESP_LOGE(TAG.data(), "Failed to add rfm95 device to spi");
  }

  hw::Gpio rst{4};
  hw::Gpio dio0{17};
  radio::Rfm95 rfm95{{rst, dio0, spi, spiRfm95Handle}};
  errorCode = rfm95.init(radio::Rfm95::Modulation::LORA);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init rfm95");
  }

  radio::Rfm95::ModemSettings settings{};
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
    ESP_LOGE(TAG.data(), "Failed to set all rfm95 settings");
  }

  storage::hw::NvsStore storage{"storage"};

  timer::sw::Timer wifiReconnectTimer;
  errorCode = wifiReconnectTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init wifi reconnect timer");
  }

  timer::sw::Timer radiorequestTimer;
  errorCode = radiorequestTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init radio request timer");
  }

  timer::sw::Timer radioTimeoutTimer;
  errorCode = radioTimeoutTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init radio timeout timer");
  }

  hw::Gpio redPin{12};
  hw::Gpio greenPin{27};
  hw::Gpio bluePin{26};
  ui::Ws2812b led{{redPin, greenPin, bluePin}};
  errorCode = led.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init led");
  }

  hw::Gpio buttonPin{34};
  ui::Button button{buttonPin};
  errorCode = button.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init button");
  }

  sw::Queue<def::ui::LedEvent> ledEventQueue{5};
  errorCode = ledEventQueue.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to led event queue");
  }

  timer::sw::Timer ledTimer;
  errorCode = ledTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to led timer");
  }

  app::UiThread uiThread{{button, led, ledEventQueue, ledTimer}};
  errorCode = uiThread.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to start UiThread");
  }

  sw::Queue<common::Telemetry> telemetryQueue{5};
  errorCode = telemetryQueue.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init telemetry queue");
  }

  app::RadioThreadHub radioThread{{rfm95, radiorequestTimer, radioTimeoutTimer,
                                   ledEventQueue, telemetryQueue}};
  errorCode = radioThread.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to start RadioThread");
  }

  sw::EventGroup connectionEventGroup;
  errorCode = connectionEventGroup.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init connection event group");
  }

  app::WifiController wifiController{
      {wifiReconnectTimer, storage, ledEventQueue, connectionEventGroup}};
  errorCode = wifiController.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init WifiController");
  }

  errorCode = wifiController.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to start WifiController");
  }

  std::string clientId{};
  storage.getString(def::key::aws::CLIENT_ID, clientId);
  std::string hostUrl{};
  storage.getString(def::key::aws::HOST_URL, hostUrl);
  ESP_LOGI(TAG.data(), "Client id: %s, host url: %s", clientId.data(),
           hostUrl.data());

  app::AwsIotClient awsIotClient{clientId};
  errorCode =
      awsIotClient.init(hostUrl.data(), {binaryAmazonRootCA1, binaryCertificate,
                                         binaryPrivateKey});
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to init AwsIotClient");
  }

  timer::sw::Timer awsiotReconnectTimer;
  errorCode = awsiotReconnectTimer.init();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to aws iot reconnect timer");
  }

  app::AwsIotThread awsThread{{awsIotClient, connectionEventGroup,
                               telemetryQueue, ledEventQueue,
                               awsiotReconnectTimer}};
  errorCode = awsThread.start();
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Failed to start AwsIotThread");
  }

  while (1) {
    sw::delayMs(10);
  }
}
}
