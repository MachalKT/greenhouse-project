#pragma once

#include <string_view>

namespace def {
namespace key {
static constexpr std::string_view SSID_SIZE{"ssid_size"};
static constexpr std::string_view PASSWORD_SIZE{"pass_size"};
static constexpr std::string_view SSID{"ssid"};
static constexpr std::string_view PASSWORD{"pass"};
} // namespace key

namespace ui {
enum class LedEvent : uint8_t {
  WIFI_CONNECTION,
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
  RADIO_CONNECTED,
  RADIO_DISCONNECTED
};

} // namespace ui
} // namespace def
