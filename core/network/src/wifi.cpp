#include "wifi.hpp"

#include "esp_event.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <array>
#include <cstring>
#include <string_view>

namespace {
static constexpr std::string_view TAG{"Wifi"};
static constexpr std::string_view SEPARATOR_AP_LIST{
    "---------------------------------+---------+------+----------------"};
static constexpr std::string_view SEPARATOR_STA_LIST{"-----------------"};
} // namespace

namespace net {
Wifi& Wifi::getIstance() {
  static Wifi instance;
  return instance;
}

common::Error Wifi::init(const Config config) {
  common::Error errorCode = preInit_();
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode = setMode_(config.mode);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode = setConfig_(config);
  if (errorCode != common::Error::OK) {
    mode_ = Mode::NONE;
    return errorCode;
  }

  return common::Error::OK;
}

common::Error Wifi::deinit() {
  esp_err_t espErrorCode = esp_wifi_deinit();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::restore(const Config config) {
  if (mode_ == Mode::NONE) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = esp_wifi_restore();
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  common::Error errorCode = setMode_(config.mode);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  errorCode = setConfig_(config);
  if (errorCode != common::Error::OK) {
    return errorCode;
  }

  return common::Error::OK;
}

common::Error Wifi::setWifiEventHandler(EventHandlerFunction eventHandler) {
  esp_err_t espErrorCode = esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, eventHandler, this, NULL);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::setIpEventHandler(EventHandlerFunction eventHandler) {
  esp_err_t espErrorCode = esp_event_handler_instance_register(
      IP_EVENT, IP_EVENT_STA_GOT_IP, eventHandler, this, NULL);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::start() {
  esp_err_t espErrorCode = esp_wifi_start();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::stop() {
  esp_err_t espErrorCode = esp_wifi_stop();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::connect() {
  if (not isStaMode_()) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = esp_wifi_connect();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::disconnect() {
  if (not isStaMode_()) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = esp_wifi_disconnect();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

void Wifi::showAvailableApList() {
  if (not isStaMode_()) {
    ESP_LOGE(TAG.data(), "Sta mode is not set");
    return;
  }

  esp_err_t espErrorCode = esp_wifi_scan_start(NULL, true);
  if (espErrorCode != ESP_OK) {
    ESP_LOGE(TAG.data(), "Scan failed");
    return;
  }

  std::array<wifi_ap_record_t, SCAN_LIST_SIZE> apInfo{};
  std::memset(apInfo.data(), 0, sizeof(apInfo));
  uint16_t number{SCAN_LIST_SIZE};

  espErrorCode = esp_wifi_scan_get_ap_records(&number, apInfo.data());
  if (espErrorCode != ESP_OK) {
    ESP_LOGE(TAG.data(), "Not download ap's list");
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
             toString_(static_cast<AuthenticateMode>(val.authmode)).data());
  }
  ESP_LOGI(TAG.data(), "%s", SEPARATOR_AP_LIST.data());
}

void Wifi::showConnectedStaList() {
  if (not isApMode_()) {
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

  ESP_LOGI(TAG.data(), "%s", SEPARATOR_STA_LIST.data());
  ESP_LOGI(TAG.data(), "   Mac Address   ");
  ESP_LOGI(TAG.data(), "%s", SEPARATOR_STA_LIST.data());
  for (int i{0}; i < list.num; ++i) {
    ESP_LOGI(TAG.data(), MACSTR, MAC2STR(list.sta->mac));
  }
  ESP_LOGI(TAG.data(), "%s", SEPARATOR_STA_LIST.data());
}

void Wifi::showIp(EventData eventData) {
  if (eventData == nullptr) {
    ESP_LOGE(TAG.data(), "IP cannot be read");
  }

  ip_event_got_ip_t* eventGotIp = static_cast<ip_event_got_ip_t*>(eventData);
  ESP_LOGI(TAG.data(), "got ip: " IPSTR, IP2STR(&eventGotIp->ip_info.ip));
}

common::Error Wifi::preInit_() {
  common::Error errorCode = nvsInit_();
  if (errorCode != common::Error::OK) {
    return common::Error::FAIL;
  }

  esp_err_t espErrorCode = esp_netif_init();
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = esp_event_loop_create_default();
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
  espErrorCode = esp_wifi_init(&wifiConfig);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Wifi::nvsInit_() {
  esp_err_t espErrorCode = nvs_flash_init();
  if (espErrorCode == ESP_OK) {
    return common::Error::OK;
  }

  espErrorCode = nvs_flash_erase();
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = nvs_flash_init();
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error Wifi::setMode_(const Mode mode) {
  esp_err_t espErrorCode = esp_wifi_set_mode(static_cast<wifi_mode_t>(mode));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  mode_ = mode;
  return common::Error::OK;
}

common::Error Wifi::setConfig_(const Config config) {
  common::Error errorCode{common::Error::OK};

  if (isStaMode_()) {
    errorCode = setStaConfig_(config.sta);
    if (errorCode != common::Error::OK) {
      ESP_LOGE(TAG.data(), "setStaConfig");
      return errorCode;
    }
  }

  if (isApMode_()) {
    errorCode = setApConfig_(config.ap);
    if (errorCode != common::Error::OK) {
      ESP_LOGE(TAG.data(), "setApConfig");
      return errorCode;
    }
  }

  return errorCode;
}

common::Error Wifi::setStaConfig_(const StaConfig config) {
  esp_netif_create_default_wifi_sta();

  wifi_config_t wifiConfig{};
  std::memcpy(wifiConfig.sta.ssid, config.ssid.data(), config.ssid.size());
  std::memcpy(wifiConfig.sta.password, config.password.data(),
              config.password.size());
  wifiConfig.sta.threshold.authmode =
      static_cast<wifi_auth_mode_t>(config.authenticateMode);

  esp_err_t espErrorCode = esp_wifi_set_config(WIFI_IF_STA, &wifiConfig);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::setApConfig_(const ApConfig config) {
  esp_netif_create_default_wifi_ap();

  wifi_config_t wifiConfig{};
  std::memcpy(wifiConfig.ap.ssid, config.ssid.data(), config.ssid.size());
  wifiConfig.ap.ssid_len = config.ssid.size();
  std::memcpy(wifiConfig.ap.password, config.password.data(),
              config.password.size());
  wifiConfig.ap.channel = config.channel;
  wifiConfig.ap.authmode =
      static_cast<wifi_auth_mode_t>(config.authenticateMode);
  wifiConfig.ap.max_connection = config.maxStaConnected;

  esp_err_t espErrorCode = esp_wifi_set_config(WIFI_IF_AP, &wifiConfig);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

bool Wifi::isStaMode_() const {
  return mode_ == Mode::STA || mode_ == Mode::APSTA;
}

bool Wifi::isApMode_() const {
  return mode_ == Mode::AP || mode_ == Mode::APSTA;
}

std::string_view Wifi::toString_(AuthenticateMode authenticateMode) {
  switch (authenticateMode) {
  case AuthenticateMode::OPEN:
    return "OPEN";
  case AuthenticateMode::WEP:
    return "WEP";
  case AuthenticateMode::WPA_PSK:
    return "WPA_PSK";
  case AuthenticateMode::WPA2_PSK:
    return "WPA2_PSK";
  case AuthenticateMode::WPA_WPA2_PSK:
    return "WPA_WPA2_PSK";
  case AuthenticateMode::ENTERPRISE:
    return "ENTERPRISE";
  case AuthenticateMode::WPA3_PSK:
    return "WPA3_PSK";
  case AuthenticateMode::WPA2_WPA3_PSK:
    return "WPA2_WPA3_PSK";
  case AuthenticateMode::WAPI_PSK:
    return "WAPI_PSK";
  case AuthenticateMode::OWE:
    return "OWE";
  case AuthenticateMode::WPA3_ENT_192:
    return "WPA3_ENT_192";
  case AuthenticateMode::DPP:
    return "DPP";
  case AuthenticateMode::WPA3_ENTERPRISE:
    return "WPA3_ENTERPRISE";
  case AuthenticateMode::WPA2_WPA3_ENTERPRISE:
    return "WPA2_WPA3_ENTERPRISE";
  }

  return "UNKNOWN";
}

} // namespace net
