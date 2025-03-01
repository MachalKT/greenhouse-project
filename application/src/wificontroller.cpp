#include "wificontroller.hpp"
#include "defs.hpp"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_wifi_types.h"
#include "wifi.hpp"
#include <cstring>

namespace {
static std::string_view TAG{"WIFI_CONTROLLER"};
static constexpr std::string_view SEPARATOR_AP_LIST{
    "---------------------------------+---------+------+----------------"};
static constexpr std::string_view SEPARATOR_STA_LIST{"-----------------"};
} // namespace

namespace app {

WifiController::WifiController(Config config) : config_{config} {
  config_.reconnectTimer.setCallback(
      [](void* arg) {
        assert(arg);
        WifiController* wifiController = static_cast<WifiController*>(arg);
        wifiController->getWifiInstance_().connect();
        ++wifiController->reconnectAttempt_;
      },
      this);
}

common::Error WifiController::init() {
  std::string ssid{""};
  std::string password{""};

  common::Error errorCode =
      readWifiCredential_(ssid, def::key::SSID, def::key::SSID_SIZE);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Cannot read ssid from nvs");
    return common::Error::FAIL;
  }

  errorCode = readWifiCredential_(password, def::key::PASSWORD,
                                  def::key::PASSWORD_SIZE);
  if (errorCode != common::Error::OK) {
    ESP_LOGE(TAG.data(), "Cannot read password from nvs");
    return common::Error::FAIL;
  }

  net::Wifi::Config wifiConfig{};
  wifiConfig.sta.ssid = ssid;
  wifiConfig.sta.password = password;
  wifiConfig.sta.authenticateMode = net::Wifi::AuthenticateMode::WPA2_PSK;
  wifiConfig.ap.ssid = "esp";
  wifiConfig.ap.password = "123456789";
  wifiConfig.ap.authenticateMode = net::Wifi::AuthenticateMode::WPA2_PSK;
  wifiConfig.ap.maxStaConnected = 2;
  wifiConfig.ap.channel = 1;
  wifiConfig.mode = net::Wifi::Mode::APSTA;

  errorCode = getWifiInstance_().init(wifiConfig);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = getWifiInstance_().setWifiEventHandler(wifiEventHandler_, this);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = getWifiInstance_().setIpEventHandler(ipEventHandler_, this);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
};

common::Error WifiController::start() { return getWifiInstance_().start(); }

common::Error WifiController::stop() { return getWifiInstance_().stop(); }

void WifiController::wifiEventHandler_(common::Argument arg,
                                       common::event::Base eventbase,
                                       common::event::Id eventId,
                                       common::event::Data eventData) {
  assert(arg);
  WifiController* wifiController = static_cast<WifiController*>(arg);

  switch (eventId) {
  case WIFI_EVENT_SCAN_DONE:
    wifiController->showAvailableApList_();
    break;
  case WIFI_EVENT_STA_START:
    wifiController->getWifiInstance_().connect();
    break;
  case WIFI_EVENT_STA_CONNECTED:
    wifiController->reconnectAttempt_ = 0;
    wifiController->config_.reconnectTimer.stop();
    break;
  case WIFI_EVENT_STA_DISCONNECTED:
    wifiController->getWifiInstance_().scan();
    wifiController->reconnect_();
    break;
  case WIFI_EVENT_AP_STACONNECTED: {
    wifiController->showConnectedStaInfo_(eventData);
  } break;
  case WIFI_EVENT_AP_STADISCONNECTED: {
    wifiController->showDisconnectedStaInfo_(eventData);
  } break;
  }
}

void WifiController::ipEventHandler_(common::Argument arg,
                                     common::event::Base eventbase,
                                     common::event::Id eventId,
                                     common::event::Data eventData) {
  assert(arg);
  WifiController* wifiController = static_cast<WifiController*>(arg);

  if (eventId == IP_EVENT_STA_GOT_IP) {
    wifiController->showIp_(eventData);
  }
}

net::Wifi& WifiController::getWifiInstance_() {
  return net::Wifi::getInstance();
}

common::Error WifiController::reconnect_() {
  if (reconnectAttempt_ > MAX_RECONNECT_ATTEMPTS) {
    reconnectAttempt_ = 0;
    return config_.reconnectTimer.startOnce(RECONNECT_TIME_US);
  }

  ++reconnectAttempt_;
  return getWifiInstance_().connect();
}

void WifiController::showAvailableApList_() {
  if (not getWifiInstance_().isStaMode()) {
    ESP_LOGE(TAG.data(), "Wifi is not set to STA mode");
  }
  std::array<wifi_ap_record_t, AP_LIST_SIZE> apInfo{};
  std::memset(apInfo.data(), 0, sizeof(apInfo));
  uint16_t number{AP_LIST_SIZE};

  esp_err_t espErrorCode = esp_wifi_scan_get_ap_records(&number, apInfo.data());
  if (espErrorCode != ESP_OK) {
    ESP_LOGE(TAG.data(), "Not download AP list");
    return;
  }

  if (number == NO_GET_AVAILABLE_AP) {
    ESP_LOGW(TAG.data(), "AP list is empty");
    return;
  }

  ESP_LOGI(TAG.data(), "%s", SEPARATOR_AP_LIST.data());
  ESP_LOGI(
      TAG.data(),
      "               SSID              | Channel | RSSI | Authentication");
  ESP_LOGI(TAG.data(), "%s", SEPARATOR_AP_LIST.data());
  for (uint16_t i{0}; i < number; ++i) {
    wifi_ap_record_t& val = apInfo.at(i);
    ESP_LOGI(TAG.data(), "%32s | %7d | %4d | %15s",
             reinterpret_cast<char*>(val.ssid), val.primary, val.rssi,
             toString_(static_cast<net::Wifi::AuthenticateMode>(val.authmode))
                 .data());
  }
  ESP_LOGI(TAG.data(), "%s", SEPARATOR_AP_LIST.data());
}

void WifiController::showConnectedStaList_() {
  if (not getWifiInstance_().isApMode()) {
    ESP_LOGE(TAG.data(), "AP mode is not set");
    return;
  }

  wifi_sta_list_t list;
  esp_err_t espErrorCode = esp_wifi_ap_get_sta_list(&list);
  if (espErrorCode != ESP_OK) {
    ESP_LOGE(TAG.data(), "Not download connected sta's list");
    return;
  }

  if (list.num == NO_STA_IS_CONNECTED) {
    ESP_LOGW(TAG.data(), "No sta is connected");
    return;
  }
}

void WifiController::showIp_(common::event::Data eventData) {
  if (eventData == nullptr) {
    ESP_LOGE(TAG.data(), "IP cannot be read");
  }

  ip_event_got_ip_t* eventGotIp = static_cast<ip_event_got_ip_t*>(eventData);
  ESP_LOGI(TAG.data(), "Got ip: " IPSTR, IP2STR(&eventGotIp->ip_info.ip));
}

void WifiController::showConnectedStaInfo_(common::event::Data eventData) {
  if (eventData == nullptr) {
    ESP_LOGE(TAG.data(), "Wrong event data");
  }

  wifi_event_ap_staconnected_t* event =
      static_cast<wifi_event_ap_staconnected_t*>(eventData);
  ESP_LOGI(TAG.data(), "Station " MACSTR " join, AID=%d", MAC2STR(event->mac),
           event->aid);
}

void WifiController::showDisconnectedStaInfo_(common::event::Data eventData) {
  if (eventData == nullptr) {
    ESP_LOGE(TAG.data(), "Wrong event data");
  }

  wifi_event_ap_stadisconnected_t* event =
      static_cast<wifi_event_ap_stadisconnected_t*>(eventData);
  ESP_LOGI(TAG.data(), "Station " MACSTR " leave, AID=%d, reason=%d",
           MAC2STR(event->mac), event->aid, event->reason);
}

std::string_view
WifiController::toString_(net::Wifi::AuthenticateMode authenticateMode) {
  switch (authenticateMode) {
  case net::Wifi::AuthenticateMode::OPEN:
    return "OPEN";
  case net::Wifi::AuthenticateMode::WEP:
    return "WEP";
  case net::Wifi::AuthenticateMode::WPA_PSK:
    return "WPA_PSK";
  case net::Wifi::AuthenticateMode::WPA2_PSK:
    return "WPA2_PSK";
  case net::Wifi::AuthenticateMode::WPA_WPA2_PSK:
    return "WPA_WPA2_PSK";
  case net::Wifi::AuthenticateMode::ENTERPRISE:
    return "ENTERPRISE";
  case net::Wifi::AuthenticateMode::WPA3_PSK:
    return "WPA3_PSK";
  case net::Wifi::AuthenticateMode::WPA2_WPA3_PSK:
    return "WPA2_WPA3_PSK";
  case net::Wifi::AuthenticateMode::WAPI_PSK:
    return "WAPI_PSK";
  case net::Wifi::AuthenticateMode::OWE:
    return "OWE";
  case net::Wifi::AuthenticateMode::WPA3_ENT_192:
    return "WPA3_ENT_192";
  case net::Wifi::AuthenticateMode::DPP:
    return "DPP";
  case net::Wifi::AuthenticateMode::WPA3_ENTERPRISE:
    return "WPA3_ENTERPRISE";
  case net::Wifi::AuthenticateMode::WPA2_WPA3_ENTERPRISE:
    return "WPA2_WPA3_ENTERPRISE";
  }

  return "UNKNOWN";
}
common::Error
WifiController::readWifiCredential_(std::string& value,
                                    const std::string_view valueKey,
                                    const std::string_view valueSizeKey) {
  uint8_t valueSize{0};

  common::Error errorCode = config_.storage.getItem(valueSizeKey, valueSize);
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  errorCode = config_.storage.getString(valueKey, value,
                                        static_cast<size_t>(valueSize));
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

} // namespace app
