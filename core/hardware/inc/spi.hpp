#pragma once

#include "ispi.hpp"
#include "types.hpp"
#include <cstdint>

namespace hw {

/**
 * @class Spi
 * @brief Class representing an SPI interface.
 */
class Spi final : public ISpi {
  public:
    /**
     * @brief Enum representing different SPI hosts.
     */
    enum class Host : uint8_t { SPI, HSPI, VSPI };

    /**
     * @brief Enum representing different DMA channels.
     */
    enum class DmaChannel : uint8_t {
      DISABLED,
      CHANNEL_1,
      CHANNEL_2,
      CHANNEL_3,
      AUTO // DMA auto selection
    };

    /**
     * @brief Configuration structure for SPI.
     */
    struct Config {
        IGpio& miso;
        IGpio& mosi;
        IGpio& sck;
        Host host;
    };

    /**
     * @brief Construct a new Spi object.
     *
     * @param config Configuration for the SPI.
     */
    Spi(Spi::Config config);

    /**
     * @brief Initialize the SPI interface.
     *
     * @param dmaChannel DMA channel to be used.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init(const DmaChannel dmaChannel);

    /**
     * @brief Add a device to the SPI bus.
     *
     * @param csPin Chip select pin for the device.
     * @param clockSpeedHz Clock speed in Hz.
     *
     * @return
     *   - SpiDeviceHandle Handle for the added device if success.
     *   - nullptr If fail.
     */
    SpiDeviceHandle addDevice(IGpio& csPin, const int clockSpeedHz);

    /**
     * @brief Write data to the SPI device.
     *
     * @param deviceHandle Handle for the SPI device.
     * @param registerAddress Register address to write to.
     * @param buffer Pointer to the buffer containing the data to be written.
     * @param bufferLength Length of the buffer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error write(SpiDeviceHandle& deviceHandle,
                        const uint8_t registerAddress, const uint8_t* buffer,
                        const size_t bufferLength) override;

    /**
     * @brief Read data from the SPI device.
     *
     * @param deviceHandle Handle for the SPI device.
     * @param registerAddress Register address to read from.
     * @param buffer Pointer to the buffer where the read data will be stored.
     * @param bufferLength Length of the buffer.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error read(SpiDeviceHandle& deviceHandle,
                       const uint8_t registerAddress, uint8_t* buffer,
                       const size_t bufferLength) override;

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
