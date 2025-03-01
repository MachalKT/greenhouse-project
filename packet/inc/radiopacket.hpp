#pragma once

#include "types.hpp"
#include <cstddef>

namespace packet {
namespace radio {

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

/**
 * @class Parser
 * @brief Class responsible for parsing radio packets.
 */
class Parser {
  public:
    /**
     * @brief Get the type of the packet from the buffer.
     *
     * @param buffer Pointer to the buffer containing the packet data.
     *
     * @return Type of the packet.
     */
    static Type getType(const uint8_t* buffer);

    /**
     * @brief Parse a request type to bytes.
     *
     * @param type Type of the packet.
     * @param buffer Pointer to the buffer where the bytes will be written.
     * @param bufferLength Length of the buffer.
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    static common::Error parseRequestToBytes(const Type type, uint8_t* buffer,
                                             const size_t bufferLength);

    /**
     * @brief Template function to parse data to bytes.
     *
     * @tparam T Type of the data to be parsed.
     * @param type Type of the packet.
     * @param data Data to be parsed.
     * @param buffer Pointer to the buffer where the bytes will be written.
     * @param bufferLength Length of the buffer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    template <typename T>
    common::Error parseToBytes(const Type& type, T& data, uint8_t* buffer,
                               const size_t bufferLength);

    /**
     * @brief Template function to parse data from bytes.
     *
     * @tparam T Type of the data to be parsed.
     * @param type Type of the packet.
     * @param data Data to be parsed.
     * @param buffer Pointer to the buffer containing the bytes.
     * @param bufferLength Length of the buffer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    template <typename T>
    common::Error parseFromBytes(const Type& type, T& data,
                                 const uint8_t* buffer,
                                 const size_t bufferLength);

    static constexpr size_t TYPE_INDEX{0};
};

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
    common::Error parseToBytes(uint8_t* buffer, const size_t bufferLength);

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
    common::Error parseFromBytes(const uint8_t* buffer,
                                 const size_t bufferLength);

  private:
    common::Telemetry telemetry_;
    const Type type_{Type::TELEMETRY};
    Parser parser;
};

} // namespace radio
} // namespace packet
