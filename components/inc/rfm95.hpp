#pragma once

#include "iradio.hpp"
#include "ispi.hpp"
#include "sx127xmodem.hpp"
#include <limits>
#include <memory>

namespace radio {
/**
 * @class Rfm95
 * @brief Rfm95 class
 */
class Rfm95 final : public IRadio {
  public:
    using Mode = sx127x::Mode;
    using Modulation = sx127x::Modulation;
    using Gain = sx127x::Gain;
    using Bandwidth = sx127x::Bandwidth;
    using SF = sx127x::SF;
    using PaPin = sx127x::PaPin;

    /**
     * @brief Configuration for the Rfm95
     */
    struct Config {
        hw::IGpio& reset;
        hw::IGpio& dio0;
        hw::ISpi& spi;
        hw::SpiDeviceHandle& spiDeviceHandle;
    };

    /**
     * @brief Modem settings
     */
    struct ModemSettings {
        uint64_t frequencyHz;
        uint16_t preambleLength;
        Gain gain;
        Bandwidth bandwidth;
        SF spreadingFactor;
        uint8_t syncWord;
        PaPin paPin;
        int8_t power;
        bool lnaBoostHfEnable;
    };

    /**
     * @brief Construct a new Rfm95 object
     *
     * @param config Configuration for the Rfm95
     */
    Rfm95(Config config);

    /**
     * @brief Initialize the Rfm95
     *
     * @param modulation Modulation type
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error init(Modulation modulation);

    /**
     * @brief Send data
     *
     * @param data Data to send
     * @param dataLength Data length
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error send(const uint8_t* data, const size_t dataLength) override;

    /**
     * @brief Receive data
     *
     * @param data Data to receive
     * @param dataLength Data length
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error receive(uint8_t* data, const size_t dataLength) override;

    /**
     * @brief Get the receive data length
     *
     * @return
     *   - Data length if success
     *   - 0 if failed
     */
    size_t getReceiveDataLength() override;

    /**
     * @brief Start listening for incoming packets
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error listening() override;

    /**
     * @brief Set interrupt request event callback
     *
     * @param cb Callback function
     * @param arg Argument for the callback
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setIrqEventCallback(common::Callback cb,
                                      common::Argument arg) override;

    /**
     * @brief Get the Interrup request event Value
     *
     * @return
     *   - common::radio::IrqEvent::RX_DONE: Packet reception complete
     *   - common::radio::IrqEvent::TX_DONE: FIFO Payload transmission complete
     *   - common::radio::IrqEvent::UNKNOWN: Unknown event
     */
    common::radio::IrqEvent getIrqEvent() override;

    /**
     * @brief Get the Signal Quality
     *
     * @return
     *   - common::SignalQuality Signal quality if success
     *   - variables in common::SignalQuality has INVALID_VALUE if failed
     */
    common::SignalQuality getSignalQuality() override;

    /**
     * @brief Set all settings
     *
     * @param settings Modem settings
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setAllSettings(ModemSettings settings);

    /**
     * @brief Set frequency for RX or TX
     *
     * @param frequency Frequency in hz
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setFrequency(uint64_t frequencyHz);

    /**
     * @brief Reset chip's memory pointers to 0. Both RX and TX
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    // common::Error resetFifo();

    /**
     * @brief Boost LNA current in high frequency mode (over 525Mhz).
     *
     * @param enable Enable or disable (true or false)
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setLnaBoostHf(bool enable);

    /**
     * @brief Set lna gain
     *
     * @param gain Type of gain (Gain::G1 - Gain::G6 or Gain::AUTO)
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setLnaGain(Gain gain);

    /**
     * @brief Set signal bandwidth
     *
     * @param bandwidth Type of bandwidth (Bandwidth::BW_7800 -
     * Bandwidth::BW_500000)
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setBandwidth(Bandwidth bandwidth);

    /**
     * @brief Set header.
     * @note Set explicit header.
     *
     * @return
     *   - common::Error::OK: Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setHeader();

    /**
     * @brief Set speading factor (SF rate)
     * @note Don't have to use SF::SF_6
     *
     * @param spreadingFactor Type of SF (SF::SF_7 - SF::SF_12)
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setModemConfig(SF spreadingFactor);

    /**
     * @brief Set syncword
     *
     * @param value Sync word value.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setSyncWord(uint8_t value);

    /**
     * @brief Set preamble length
     *
     * @param length Preamble length in symbols for LoRa and bytes for FSK/OOK
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setPreambleLength(uint16_t length);

    /**
     * @brief Set output power for transmittion
     *
     * @param pin Type of PaPin (PaPin::RFO or PaPin::BOOST)
     * @param Output power in dbm. if RFO pin range: from -4 to 15dbm, if
     * PA_BOOST range is from 2 to 17dbm. And also 20dbm for high power output.
     *
     * @return
     *   - common::Error::OK Success.
     *   - common::Error::FAIL: Fail.
     */
    common::Error setPaConfig(PaPin pin, int8_t power);

  private:
    common::Error setModem_(Modulation& modulation);

    common::Error setMode_(Mode mode);

    Config config_;
    std::unique_ptr<sx127x::ModemBase> modem_{nullptr};
};
} // namespace radio
