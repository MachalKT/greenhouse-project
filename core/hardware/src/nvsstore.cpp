#include "nvsstore.hpp"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs_handle.hpp"
#include <vector>

namespace storage {
namespace hw {
bool NvsStore::isNvsInitialized_{false};

NvsStore::NvsStore(const std::string_view nvsNamepsace)
    : namespace_{nvsNamepsace} {}

common::Error NvsStore::init() {
  if (isNvsInitialized_) {
    return common::Error::OK;
  }

  esp_err_t espErrorCode = nvs_flash_init();
  if (espErrorCode == ESP_OK) {
    isNvsInitialized_ = true;
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

  isNvsInitialized_ = true;
  return common::Error::OK;
}

common::Error NvsStore::erease() {
  esp_err_t espErrorCode = nvs_flash_erase();
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  isNvsInitialized_ = false;
  return common::Error::OK;
}

common::Error NvsStore::setItem(const std::string_view& key,
                                const uint8_t& item) {
  esp_err_t espErrorCode{ESP_OK};

  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(namespace_.data(), NVS_READWRITE, &espErrorCode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = handle->set_item(key.data(), item);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error NvsStore::getItem(const std::string_view& key, uint8_t& item) {
  esp_err_t espErrorCode{ESP_OK};

  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(namespace_.data(), NVS_READWRITE, &espErrorCode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = handle->get_item(key.data(), item);
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

common::Error NvsStore::setString(const std::string_view& key,
                                  const std::string& string) {
  esp_err_t espErrorCode{ESP_OK};

  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(namespace_.data(), NVS_READWRITE, &espErrorCode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  const std::string stringSizeKey{std::string(key) +
                                  STRING_SIZE_KEY_SUFFIX.data()};
  espErrorCode = handle->set_item(stringSizeKey.data(),
                                  static_cast<uint8_t>(string.size()));
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = handle->set_string(key.data(), string.data());
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  return common::Error::OK;
}

common::Error NvsStore::getString(const std::string_view& key,
                                  std::string& string) {
  esp_err_t espErrorCode{ESP_OK};

  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(namespace_.data(), NVS_READWRITE, &espErrorCode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  const std::string stringSizeKey{std::string(key) +
                                  STRING_SIZE_KEY_SUFFIX.data()};
  uint8_t stringSize{0};
  espErrorCode = handle->get_item(stringSizeKey.data(), stringSize);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }
  stringSize = stringSize + 1;

  std::vector<char> buffer(stringSize, '\0');
  espErrorCode = handle->get_string(key.data(), buffer.data(), stringSize);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  string.assign(buffer.data());
  return common::Error::OK;
}

common::Error NvsStore::save() {
  esp_err_t espErrorCode{ESP_OK};

  std::unique_ptr<nvs::NVSHandle> handle =
      nvs::open_nvs_handle(namespace_.data(), NVS_READWRITE, &espErrorCode);
  if (espErrorCode != ESP_OK) {
    return common::Error::FAIL;
  }

  espErrorCode = handle->commit();
  return espErrorCode == ESP_OK ? common::Error::OK : common::Error::FAIL;
}

} // namespace hw
} // namespace storage
