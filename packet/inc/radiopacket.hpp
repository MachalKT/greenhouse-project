#pragma once

#include "types.hpp"
#include <cstddef>

namespace packet {
namespace radio {
static constexpr size_t TYPE_INDEX{0};

/**
 * @brief Enum class representing different types of radio packets.
 */
enum class Type : uint8_t {
  UNKNOWN,           // Unknown packet type
  OK,                // Packet indicating OK status
  NOT_OK,            // Packet indicating NOT OK status
  TELEMETRY_REQUEST, // Packet requesting telemetry data
  TELEMETRY          // Packet containing telemetry data
};

namespace utils {
Type getType(const uint8_t* buffer);

common::Error serializeRequest(const Type type, uint8_t* buffer,
                               const size_t bufferLength);
} // namespace utils

/**
 * @class Telemetry
 * @brief Class representing telemetry data in a radio packet.
 */
class __attribute__((packed)) Telemetry {
  public:
    /**
     * @brief Construct a new Telemetry object.
     *
     * @param telemetry Telemetry data.
     */
    explicit Telemetry(common::Telemetry telemetry);

    /**
     * @brief Get the telemetry data.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Telemetry getTelemetry() const;

    /**
     * @brief Parse telemetry data to bytes.
     *
     * @param buffer Pointer to the buffer where the bytes will be written.
     * @param bufferLength Length of the buffer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error serialize(uint8_t* buffer, const size_t bufferLength);

    /**
     * @brief Parse telemetry data from bytes.
     *
     * @param buffer Pointer to the buffer containing the bytes.
     * @param bufferLength Length of the buffer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error deserialize(const uint8_t* buffer, const size_t bufferLength);

  private:
    common::Telemetry telemetry_;
    const Type type_{Type::TELEMETRY};
};

} // namespace radio
} // namespace packet
