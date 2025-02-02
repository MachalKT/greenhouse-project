#pragma once

#include "interfaces/ispi.hpp"

#include <cstdint>

#include "gpio.hpp"
#include "types.hpp"

namespace hw {
class Spi final : public ISpi {
  public:
    enum class Host : uint8_t { SPI, HSPI, VSPI };

    enum class DmaChannel : uint8_t {
      DISABLED,
      CHANNEL_1,
      CHANNEL_2,
      CHANNEL_3,
      AUTO,
    };

    struct Config {
        Gpio& miso;
        Gpio& mosi;
        Gpio& sck;
        Host host;
    };

    Spi(Config config);

    common::Error init(const DmaChannel dmaChannel);

    common::Error addDevice(spi::DeviceHandle& deviceHandle,
                            const common::PinNumber csPin,
                            const int clockSpeedHz) override;

    common::Error write(spi::DeviceHandle& deviceHandle, int registerAddress,
                        const uint32_t* data, size_t dataLength) override;

    common::Error write(spi::DeviceHandle& deviceHandle, int registerAddress,
                        const uint8_t* buffer, size_t bufferLength) override;

    common::Error read(spi::DeviceHandle& deviceHandle, int registerAddress,
                       uint32_t* data, size_t dataLength) override;

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
