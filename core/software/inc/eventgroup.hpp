#pragma once

#include "types.hpp"

namespace sw {
/**
 * @class EventGroup
 * @brief A class for managing event groups, allowing for setting, clearing, and
 * waiting on event bits.
 *
 * The `EventGroup` class provides a mechanism to manage a group of event bits.
 * It allows setting and clearing specific bits, waiting for specific bits to be
 * set, and retrieving the current state of the bits.
 */
class EventGroup {
  public:
    /**
     * @brief Alias for the type used to represent event bits.
     */
    using Bits = uint32_t;

    /**
     * @brief Default constructor for the EventGroup class.
     */
    EventGroup() = default;

    /**
     * @brief Destructor for the EventGroup class.
     */
    ~EventGroup();

    /**
     * @brief Initializes the event group.
     *
     * @return common::Error Error code indicating success or failure.
     *   - common::Error::OK: Initialization succeeded.
     *   - common::Error::FAIL: Initialization failed.
     */
    common::Error init();

    /**
     * @brief Sets specific bits in the event group.
     *
     * @param bits The bits to set.
     *
     * @return common::Error Error code indicating success or failure.
     *   - common::Error::OK: Bits were successfully set.
     *   - common::Error::FAIL: Failed to set bits.
     */
    common::Error set(Bits bits);

    /**
     * @brief Clears specific bits in the event group.
     *
     * @param bits The bits to clear.
     *
     * @return common::Error Error code indicating success or failure.
     *   - common::Error::OK: Bits were successfully cleared.
     *   - common::Error::FAIL: Failed to clear bits.
     */
    common::Error clear(Bits bits);

    /**
     * @brief Waits for specific bits to be set in the event group.
     *
     * @param bits The bits to wait for.
     * @param timeoutMs The timeout period (in milliseconds) to wait for the
     * bits.
     *
     * @return common::Error Error code indicating success or failure.
     *   - common::Error::OK: The specified bits were set within the timeout.
     *   - common::Error::TIMEOUT: The timeout expired before the bits were set.
     *   - common::Error::FAIL: Failed to wait for bits.
     */
    common::Error wait(Bits bits, common::Time timeoutMs);

    /**
     * @brief Retrieves the current state of all bits in the event group.
     *
     * @return Bits The current state of the event bits.
     */
    Bits getBits() const;

  private:
    /**
     * @brief Alias for the internal handler type used by the event group.
     */
    using Handler = void*;

    Bits allBits_{0};
    Handler handler_{nullptr};
};

} // namespace sw
