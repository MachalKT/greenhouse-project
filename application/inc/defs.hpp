#pragma once

#include "eventgroup.hpp"
#include <string_view>

namespace def {
namespace key {
static constexpr std::string_view SSID{"ssid"};
static constexpr std::string_view PASSWORD{"pass"};
} // namespace key

namespace ui {
enum class LedEvent : uint8_t {
  UNKNOWN,
  WIFI_CONNECTION,
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
  AWS_CONNECTED,
  AWS_DISCONNECTED,
  RADIO_COMMUNICATION,
  RADIO_TIMEOUT
};
}

namespace net {
static constexpr sw::EventGroup::Bits WIFI_CONNECTED_BIT{1 << 0};
static constexpr sw::EventGroup::Bits AWS_CONNECTED_BIT{1 << 1};
} // namespace net

} // namespace def
