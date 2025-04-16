#include "radiopacket.hpp"
#include <cstring>

namespace {} // namespace

namespace packet {
namespace radio {
namespace utils {

template <typename T>
common::Error serialize(const Type& type, const T& data, uint8_t* buffer,
                        size_t bufferLength) {
  if (bufferLength < sizeof(Type) + sizeof(T)) {
    return common::Error::FAIL;
  }

  buffer[packet::radio::TYPE_INDEX] = static_cast<uint8_t>(type);
  std::memcpy(buffer + 1, &data, sizeof(T));

  return common::Error::OK;
}

template <typename T>
common::Error deserialize(const Type& type, T& data, const uint8_t* buffer,
                          size_t bufferLength) {
  if (bufferLength < sizeof(Type) + sizeof(T)) {
    return common::Error::FAIL;
  }

  if (getType(buffer) != type) {
    return common::Error::FAIL;
  }

  std::memcpy(&data, buffer + 1, sizeof(T));

  return common::Error::OK;
}

Type getType(const uint8_t* buffer) {
  return static_cast<Type>(buffer[TYPE_INDEX]);
}

common::Error serializeRequest(const Type type, uint8_t* buffer,
                               const size_t bufferLength) {
  if (bufferLength < sizeof(Type)) {
    return common::Error::FAIL;
  }

  std::memcpy(buffer, &type, bufferLength);
  return common::Error::OK;
}

} // namespace utils

Telemetry::Telemetry(common::Telemetry telemetry) : telemetry_{telemetry} {}

common::Telemetry Telemetry::getTelemetry() const { return telemetry_; }

common::Error Telemetry::serialize(uint8_t* buffer, const size_t bufferLength) {
  return utils::serialize(type_, telemetry_, buffer, bufferLength);
}

common::Error Telemetry::deserialize(const uint8_t* buffer,
                                     const size_t bufferLength) {
  return utils::deserialize(type_, telemetry_, buffer, bufferLength);
}

} // namespace radio
} // namespace packet
