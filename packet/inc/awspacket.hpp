#pragma once

#include "types.hpp"
#include <cstddef>

namespace packet {
namespace aws {
constexpr size_t BUFFER_SIZE{256};

/**
 * @class Telemetry
 * @brief A class for managing telemetry data and converting it to JSON format.
 *
 * The `Telemetry` class is responsible for handling telemetry data and
 * providing functionality to serialize it into a JSON string for communication
 * with AWS IoT.
 */
class Telemetry {
  public:
    /**
     * @brief Constructs a `Telemetry` object with the given telemetry data.
     * @param telemetry The telemetry data to be managed.
     */
    explicit Telemetry(common::Telemetry telemetry);

    /**
     * @brief Serializes the telemetry data into a JSON string.
     * @param buffer The buffer to store the JSON string.
     * @param bufferLength The length of the provided buffer.
     * @return common::Error Error code indicating success or failure.
     *   - common::Error::OK: Serialization succeeded.
     *   - common::Error::FAIL: Serialization failed (e.g., buffer too small).
     */
    common::Error parseToJsonString(char* buffer, const size_t bufferLength);

  private:
    common::Telemetry telemetry_;
};

} // namespace aws
} // namespace packet
