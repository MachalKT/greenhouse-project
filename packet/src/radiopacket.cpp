#include "radiopacket.hpp"
#include <cstring>

namespace packet {
namespace radio {

Type Parser::getType(const uint8_t* buffer) {
  return static_cast<Type>(buffer[TYPE_INDEX]);
}

common::Error Parser::parseRequestToBytes(const Type type, uint8_t* buffer,
                                          const size_t bufferLength) {
  if (bufferLength < sizeof(Type)) {
    return common::Error::FAIL;
  }

  std::memcpy(buffer, &type, bufferLength);
  return common::Error::OK;
}

template <typename T>
common::Error Parser::parseToBytes(const Type& type, T& data, uint8_t* buffer,
                                   const size_t bufferLength) {
  if (bufferLength < sizeof(Type) + sizeof(T)) {
    return common::Error::FAIL;
  }

  buffer[TYPE_INDEX] = static_cast<uint8_t>(type);
  std::memcpy(buffer + 1, &data, sizeof(T));

  return common::Error::OK;
}

template <typename T>
common::Error Parser::parseFromBytes(const Type& type, T& data,
                                     const uint8_t* buffer,
                                     const size_t bufferLength) {
  if (bufferLength < sizeof(Type) + sizeof(T)) {
    return common::Error::FAIL;
  }

  if (getType(buffer) != type) {
    return common::Error::FAIL;
  }

  std::memcpy(&data, buffer + 1, sizeof(T));

  return common::Error::OK;
}

Telemetry::Telemetry(common::Telemetry telemetry) : telemetry_{telemetry} {}

common::Telemetry Telemetry::getTelemetry() const { return telemetry_; }

common::Error Telemetry::parseToBytes(uint8_t* buffer,
                                      const size_t bufferLength) {
  return parser.parseToBytes(type_, telemetry_, buffer, bufferLength);
}

common::Error Telemetry::parseFromBytes(const uint8_t* buffer,
                                        const size_t bufferLength) {
  return parser.parseFromBytes(type_, telemetry_, buffer, bufferLength);
}

} // namespace radio
} // namespace packet
