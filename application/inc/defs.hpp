#pragma once

#include "eventgroup.hpp"
#include <string_view>

namespace def {

namespace key {
namespace wifi {
static constexpr std::string_view STA_SSID{"ssid"};
static constexpr std::string_view STA_PASSWORD{"pass"};
} // namespace wifi

namespace aws {
static constexpr std::string_view CLIENT_ID{"client_id"};
static constexpr std::string_view HOST_URL{"host_url"};
} // namespace aws
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
