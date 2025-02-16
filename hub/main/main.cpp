#include "delay.hpp"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif_types.h"
#include "esp_wifi_types.h"
#include "types.hpp"
#include "wifi.hpp"

static constexpr uint8_t MAX_RECONNECT_ATTEMPTS{5};

static void myEventHandler(common::Argument arg, common::event::Base eventbase,
                           common::event::Id eventId,
                           common::event::Data eventData) {
  assert(arg);
  net::Wifi* wifiSta = static_cast<net::Wifi*>(arg);
  ESP_LOGW("WIFI", "eventBase %d", static_cast<int>(eventId));
  static int retryNum{0};

  switch (eventId) {
  case WIFI_EVENT_STA_START:
    ESP_LOGE("WIFI", "start");
    wifiSta->connect();
    break;
  case WIFI_EVENT_STA_CONNECTED:
    ESP_LOGE("WIFI", "connected");
    retryNum = 0;
    break;
  case WIFI_EVENT_STA_DISCONNECTED:
    ESP_LOGE("WIFI", "disconnected");
    retryNum++;
    if (retryNum >= MAX_RECONNECT_ATTEMPTS) {
      ESP_LOGE("WIFI", "recconect failed");
    }
    wifiSta->connect();
    break;
  case IP_EVENT_STA_GOT_IP:
    ESP_LOGE("WIFI", "got ip");
    wifiSta->showIp(eventData);
    break;
  case WIFI_EVENT_AP_STACONNECTED: {
    wifiSta->showConnectedStaInfo(eventData);
  } break;
  case WIFI_EVENT_AP_STADISCONNECTED: {
    wifiSta->showDisconnectedStaInfo(eventData);
  } break;
  }
}

extern "C" {
void app_main(void) {
  net::Wifi::Config config{};
  config.sta.ssid = "";
  config.sta.password = "";
  config.sta.authenticateMode = net::Wifi::AuthenticateMode::WPA2_PSK;
  config.ap.ssid = "esp";
  config.ap.password = "123456789";
  config.ap.authenticateMode = net::Wifi::AuthenticateMode::WPA2_PSK;
  config.ap.maxStaConnected = 2;
  config.ap.channel = 1;
  config.mode = net::Wifi::Mode::APSTA;

  common::Error ec = net::Wifi::getIstance().init(config);
  if (ec != common::Error::OK) {
    ESP_LOGE("MAIN", "wifi init fail");
  }

  ec = net::Wifi::getIstance().setWifiEventHandler(myEventHandler);
  if (ec != common::Error::OK) {
    ESP_LOGE("MAIN", "wifi set wifi handler fail");
  }

  ec = net::Wifi::getIstance().setIpEventHandler(myEventHandler);
  if (ec != common::Error::OK) {
    ESP_LOGE("MAIN", "wifi set ip handler fail");
  }

  ec = net::Wifi::getIstance().start();
  if (ec != common::Error::OK) {
    ESP_LOGE("MAIN", "wifi start fail");
  }

  while (1) {
    sw::delayMs(5000);
    net::Wifi::getIstance().showAvailableApList();
    net::Wifi::getIstance().showConnectedStaList();
  }
}
}
