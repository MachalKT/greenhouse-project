#pragma once
#include <cstdint>
#include <vector>

#include "igpio.hpp"
#include "types.hpp"

namespace hw {
class Gpio final : public IGpio {
  public:
    /**
     * @brief Construct a new Gpio object.
     *
     * @param number Gpio number.
     */
    explicit Gpio(const GpioNumber number);

    /**
     * @brief Gpio set mode.Gpio can be set to input or output mode.
     *
     * @param mode Gpio mode.
     *
     * @return
     *  - common::Error::OK: Success.
     *  - common::Error::FAIL: Fail.
     *  - common::Error::INVALID_STATE: Gpio is not assigned.
     */
    common::Error setMode(const GpioMode mode) override;

    /**
     * @brief Set gpio level.
     *
     * @param level Gpio level.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Gpio is not set in output mode.
     */
    common::Error setLevel(const GpioLevel level) override;

    /**
     * @brief Configure pull up or pull down resistor.
     *
     * @param pullUpEnable Pull up resistor enable.
     * @param pullDownEnable Pull down resistor enable.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     *   - common::Error::INVALID_STATE: Gpio is not assigned.
     */
    common::Error configurePullUpDown(const bool pullUpEnable,
                                      const bool pullDownEnable) override;

    /**
     * @brief Get gpio level.
     * @note if gpio is not assigned, return GpioLevel::LOW.
     *
     * @return
     *   - GpioLevel::LOW: Low level.
     *   - GpioLevel::HIGH: High level.
     */
    GpioLevel getLevel() const override;

    /**
     * @brief Get gpio number.
     * @note If gpio is not assigned, return INVALID_GPIO_NUMBER (-1).
     *
     * @return GpioNumber Gpio number.
     */
    GpioNumber getNumber() const override;

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
     *   - common::Error::INVALID_STATE: Gpio is not assigned.
     */
    common::Error setInterrupt(const GpioInterruptType interruptType,
                               common::Callback interruptCallback,
                               common::Argument callbackData) override;

    /**
     * @brief Is gpio assigned.
     *
     * @return
     *   - true: Gpio is assigned.
     *   - false: Gpio is not assigned.
     */
    bool isGpioAssigned() const override;

  private:
    /**
     * @brief Set isr service.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setIsrService_();

    static constexpr int8_t FIRST_GPIO_NUMBER{0};
    static constexpr int8_t LAST_GPIO_NUMBER{39};
    static std::vector<GpioNumber> usedGpioNumbers_;
    bool isInterruptEnabled_{false};
    int8_t number_{INVALID_GPIO_NUMBER};
    GpioMode mode_{GpioMode::DISABLE};
};

} // namespace hw
