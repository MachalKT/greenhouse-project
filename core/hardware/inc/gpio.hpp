#pragma once
#include <cstdint>
#include <vector>

#include "interfaces/igpio.hpp"
#include "types.hpp"

namespace hw {
namespace gpio {
/**
 * @brief Gpio mode.
 */
enum class Mode : uint8_t {
  DISABLE = 0,
  INPUT,
  OUTPUT,
};

/**
 * @brief Gpio level.
 */
enum class Level : uint8_t {
  LOW = 0,
  HIGH,
};

/**
 * @brief Gpio interrupt type.
 */
enum class InterruptType : uint8_t {
  DISABLE = 0,
  RISING_EDGE,
  FALLING_EDGE,
  RISING_AND_FALLING_EDGE,
  LOW_LEVEL,
  HIGH_LEVEL,
};
} // namespace gpio

class Gpio final : public IGpio {
  public:
    /**
     * @brief Construct a new Gpio object.
     */
    explicit Gpio(const gpio::PinNumber pinNumber);

    /**
     * @brief gpio set mode.Gpio can be set to input or output mode.
     *
     * @param mode Gpio mode.
     *
     * @return
     *  - common::Error::OK: Success.
     *  - common::Error::FAIL: Fail.
     *  - common::Error::INVALID_STATE: Pin is not assigned.
     */
    common::Error setMode(const gpio::Mode mode) override;

    /**
     * @brief Set gpio level.
     *
     * @param level Gpio level.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Pin is not set in output mode.
     */
    common::Error setLevel(const gpio::Level level) override;

    /**
     * @brief Configure pull up or pull down resistor.
     *
     * @param pullUpEnable Pull up resistor enable.
     * @param pullDownEnable Pull down resistor enable.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Pin is not assigned.
     */
    common::Error configurePullUpDown(const bool pullUpEnable,
                                      const bool pullDownEnable) override;

    /**
     * @brief Get gpio level.
     * @note if pin is not assigned, return gpio::Level::LOW.
     *
     * @return
     *   - gpio::Level::LOW: Low level.
     *   - gpio::Level::HIGH: High level.
     */
    gpio::Level getLevel() const override;

    /**
     * @brief Get pin number.
     * @note if pin is not assigned, return PIN_NOT_ASSIGN (-1).
     *
     * @return common::PinNumber Pin number.
     */
    gpio::PinNumber getPin() const override;

    /**
     * @brief Set interrupt.
     *
     * @param interruptType Interrupt type.
     * @param interruptCallback Interrupt callback.
     * @param callbackData Interrupt callback data.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Pin is not assigned.
     */
    common::Error setInterrupt(const gpio::InterruptType interruptType,
                               common::Callback interruptCallback,
                               common::CallbackData callbackData) override;

    /**
     * @brief Is pin assigned.
     *
     * @return
     *   - true: Pin is assigned.
     *   - false: Pin is not assigned.
     */
    bool isPinAssigned() const override;

  private:
    /**
     * @brief Set isr service.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setIsrService_();

    static constexpr int8_t PIN_NOT_ASSIGN{-1};
    static constexpr int8_t PIN_FIRST_NUMBER{0};
    static constexpr int8_t PIN_LAST_NUMBER{33};
    static std::vector<gpio::PinNumber> usedPinNumbers_;
    bool isInterruptEnabled_{false};
    int8_t pinNumber_{PIN_NOT_ASSIGN};
    gpio::Mode mode_{gpio::Mode::DISABLE};
};

} // namespace hw
