#pragma once

#include "interfaces/ispi.hpp"

#include <cstdint>

#include "types.hpp"

namespace hw {
namespace spi {
/**
 * @brief  Spi Host. Peripherals that are software-accessible in it.
 */
enum class Host : uint8_t { SPI, HSPI, VSPI };

/**
 * @brief  Spi Dma Channel.
 */
enum class DmaChannel : uint8_t {
  DISABLED,
  CHANNEL_1,
  CHANNEL_2,
  CHANNEL_3,
  AUTO,
};

} // namespace spi

class Spi final : public ISpi {
  public:
    /**
     * @brief Spi configuration.
     */
    struct Config {
        IGpio& miso;
        IGpio& mosi;
        IGpio& sck;
        spi::Host host;
    };

    /**
     * @brief Construct a new Spi object.
     */
    Spi(Spi::Config config);

    /**
     * @brief Initialize spi.
     *
     * @param dmaChannel Dma channel.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL Fail.
     */
    common::Error init(const spi::DmaChannel dmaChannel);

    /**
     * @brief Add device to spi.
     *
     * @param deviceHandle Device handle.
     * @param csPin Chip select pin.
     * @param clockSpeedHz Clock speed in Hz.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL Fail.
     *   - common::Error::INVALID_ARG Invalid argument.
     */
    common::Error addDevice(spi::DeviceHandle& deviceHandle, IGpio& csPin,
                            const int clockSpeedHz) override;

    /**
     * @brief Write data to spi device.
     *
     * @param deviceHandle Device handle.
     * @param registerAddress Register address.
     * @param buffer Buffer.
     * @param bufferLength Buffer length.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL Fail.
     */
    common::Error write(spi::DeviceHandle& deviceHandle, int registerAddress,
                        const uint8_t* buffer, size_t bufferLength) override;

    /**
     * @brief Read data from spi device.
     *
     * @param deviceHandle Device handle.
     * @param registerAddress Register address.
     * @param buffer Buffer.
     * @param bufferLength Buffer length.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL Fail.
     */
    common::Error read(spi::DeviceHandle& deviceHandle, int registerAddress,
                       uint8_t* buffer, size_t bufferLength) override;

    static constexpr double CLOCK_SPEED_HZ{3E6};

  private:
    static constexpr int DMA_ENABLE{0};
    static constexpr int NOT_USE_WRITE_PROTECT{-1};
    static constexpr int NOT_USE_HOLD{-1};
    static constexpr int QUEUE_SIZE{16};
    static constexpr int COMMAND_BITS{0};
    static constexpr int ADDRESS_BITS{8};
    static constexpr int DUMMY_BITS{0};
    static constexpr int CPOL_0_CPHA_0{0};

    Config config_;
};
} // namespace hw
