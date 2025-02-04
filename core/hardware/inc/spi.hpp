#pragma once

#include "interfaces/ispi.hpp"

#include <cstdint>

#include "types.hpp"

namespace hw {
namespace spi {
enum class Host : uint8_t { SPI, HSPI, VSPI };

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
    struct Config {
        IGpio& miso;
        IGpio& mosi;
        IGpio& sck;
        spi::Host host;
    };

    Spi(Spi::Config config);

    common::Error init(const spi::DmaChannel dmaChannel);

    common::Error addDevice(spi::DeviceHandle& deviceHandle, IGpio& csPin,
                            const int clockSpeedHz) override;

    common::Error write(spi::DeviceHandle& deviceHandle, int registerAddress,
                        const uint8_t* buffer, size_t bufferLength) override;

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
