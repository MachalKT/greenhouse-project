#include "wifi.hpp"

#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <array>
#include <cstring>
#include <string_view>

namespace net {
Wifi& Wifi::getInstance() {
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

common::Error
Wifi::setWifiEventHandler(common::event::HandlerFunction eventHandler,
                          common::Argument arg) {
  esp_err_t espErrorCode = esp_event_handler_instance_register(
      WIFI_EVENT, EVENT_ANY_ID, eventHandler, arg, NULL);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error
Wifi::setIpEventHandler(common::event::HandlerFunction eventHandler,
                        common::Argument arg) {
  esp_err_t espErrorCode = esp_event_handler_instance_register(
      IP_EVENT, EVENT_ANY_ID, eventHandler, arg, NULL);
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
  if (not isStaMode()) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = esp_wifi_connect();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::disconnect() {
  if (not isStaMode()) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = esp_wifi_disconnect();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error Wifi::scan() {
  if (not isStaMode()) {
    return common::Error::INVALID_STATE;
  }

  esp_err_t espErrorCode = esp_wifi_scan_start(NULL, true);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

bool Wifi::isStaMode() const {
  return mode_ == Mode::STA || mode_ == Mode::APSTA;
}

bool Wifi::isApMode() const {
  return mode_ == Mode::AP || mode_ == Mode::APSTA;
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

  if (isStaMode()) {
    errorCode = setStaConfig_(config.sta);
    if (errorCode != common::Error::OK) {
      return errorCode;
    }
  }

  if (isApMode()) {
    errorCode = setApConfig_(config.ap);
    if (errorCode != common::Error::OK) {
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

} // namespace net
