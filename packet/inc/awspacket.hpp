#pragma once

#include "types.hpp"
#include <cstddef>

namespace packet {
namespace aws {
constexpr size_t BUFFER_SIZE{256};

/**
 * @class Telemetry
 * @brief A class for managing telemetry data and converting it to JSON format.
 */
class Telemetry {
  public:
    /**
     * @brief Constructs a `Telemetry` object with the given telemetry data.
     *
     * @param telemetry The telemetry data to be managed.
     */
    explicit Telemetry(common::Telemetry telemetry);

    /**
     * @brief Serializes the telemetry data to JSON.
     *
     * @param buffer The buffer to store the JSON string.
     * @param bufferLength The length of the provided buffer.
     *
     * @return common::Error Error code indicating success or failure.
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error serializeToJson(char* buffer, const size_t bufferLength);

  private:
    common::Telemetry telemetry_;
};

} // namespace aws
} // namespace packet
