#pragma once

#include <cstdint>

// Register definitions for SX127x based on datasheet:
// https://www.alldatasheet.com/datasheet-pdf/view/800243/SEMTECH/SX127X.html

namespace sx127x {
namespace reg {
namespace common {
constexpr uint8_t FIFO{0x00};    // FIFO read/write access
constexpr uint8_t OP_MODE{0x01}; // Operating mode & LoRa/FSK selection
constexpr uint8_t FRF_MSB{0x06}; // RF Carrier Frequency, Most Significant Bits
constexpr uint8_t FRF_MID{0x07}; // RF Carrier Frequency, Intermediate Bits
constexpr uint8_t FRF_LSB{0x08}; // RF Carrier Frequency, Least Significant Bits
constexpr uint8_t PA_CONFIG{0x09}; // PA selection and Output Power control
constexpr uint8_t PA_RAMP{0x0A}; // Control of PA ramp time, low phase noise PLL
constexpr uint8_t OCP{0x0B};     // Over Current Protection control
constexpr uint8_t LNA{0x0C};     // LNA settings
constexpr uint8_t DIO_MAPPING_1{0x40}; // Mapping of pins DIO0 to DIO3
constexpr uint8_t DIO_MAPPING_2{0x41}; // Mapping of pins DIO4 and DIO5
constexpr uint8_t VERSION{0x42};       // Module version
constexpr uint8_t TCXO{0x4B};          // TCXO configuration
constexpr uint8_t PA_DAC{0x4D};        // PA DAC configuration
constexpr uint8_t FORMER_TEMP{0x5B};   // Stored temperature value
constexpr uint8_t AGC_REF{0x61};       // AGC reference
constexpr uint8_t AGC_THRESH_1{0x62};  // AGC threshold 1
constexpr uint8_t AGC_THRESH_2{0x63};  // AGC threshold 2
constexpr uint8_t AGC_THRESH_3{0x64};  // AGC threshold 3
constexpr uint8_t PLL{0x70};           // Control of the PLL bandwidth

namespace mask {
constexpr uint8_t LNA_BOOST_HF{0b11111100};
constexpr uint8_t LNA_GAIN{0b00011111};
constexpr uint8_t DIO_MAPPING_1{0b00111111};
} // namespace mask
} // namespace common

namespace fsk {
constexpr uint8_t BITRATE_MSB{0x02}; // Bit Rate setting, Most Significant Bits
constexpr uint8_t BITRATE_LSB{0x03}; // Bit Rate setting, Least Significant Bits
constexpr uint8_t FDEV_MSB{0x04};    // Frequency Deviation setting, MSB
constexpr uint8_t FDEV_LSB{0x05};    // Frequency Deviation setting, LSB
constexpr uint8_t RX_CONFIG{0x0D};   // AFC, AGC, control
constexpr uint8_t RSSI_CONFIG{0x0E}; // RSSI
constexpr uint8_t RSSI_COLLISION{0x0F};  // RSSI Collision detector
constexpr uint8_t RSSI_THRESH{0x10};     // RSSI Threshold control
constexpr uint8_t RSSI_VALUE{0x11};      // RSSI value in dBm
constexpr uint8_t RX_BW{0x12};           // Channel Filter BW Control
constexpr uint8_t AFC_BW{0x13};          // AFC Channel Filter BW
constexpr uint8_t OOK_PEAK{0x14};        // OOK demodulator
constexpr uint8_t OOK_FIX{0x15};         // Threshold of the OOK demod
constexpr uint8_t OOK_AVG{0x16};         // Average of the OOK demod
constexpr uint8_t AFC_FEI{0x1A};         // AFC and FEI control
constexpr uint8_t AFC_MSB{0x1B};         // AFC correction MSB
constexpr uint8_t AFC_LSB{0x1C};         // AFC correction LSB
constexpr uint8_t FEI_MSB{0x1D};         // Frequency error MSB
constexpr uint8_t FEI_LSB{0x1E};         // Frequency error LSB
constexpr uint8_t PREAMBLE_DETECT{0x1F}; // Settings of the Preamble Detector
constexpr uint8_t RX_TIMEOUT_1{0x20};    // Timeout Rx request and RSSI
constexpr uint8_t RX_TIMEOUT_2{0x21};    // Timeout RSSI and PayloadReady
constexpr uint8_t RX_TIMEOUT_3{0x22};    // Timeout RSSI and SyncAddress
constexpr uint8_t RX_DELAY{0x23};        // Delay between Rx cycles
constexpr uint8_t OSC{0x24}; // RC Oscillators Settings, CLKOUT frequency
constexpr uint8_t PREAMBLE_MSB{0x25};    // Preamble length, MSB
constexpr uint8_t PREAMBLE_LSB{0x26};    // Preamble length, LSB
constexpr uint8_t SYNC_CONFIG{0x27};     // Sync Word Recognition control;
constexpr uint8_t SYNC_VALUE_1{0x28};    // Sync Word bytes 1
constexpr uint8_t SYNC_VALUE_2{0x29};    // Sync Word bytes 2
constexpr uint8_t SYNC_VALUE_3{0x2A};    // Sync Word bytes 3
constexpr uint8_t SYNC_VALUE_4{0x2B};    // Sync Word bytes 4
constexpr uint8_t SYNC_VALUE_5{0x2C};    // Sync Word bytes 5
constexpr uint8_t SYNC_VALUE_6{0x2D};    // Sync Word bytes 6
constexpr uint8_t SYNC_VALUE_7{0x2E};    // Sync Word bytes 7
constexpr uint8_t SYNC_VALUE_8{0x2F};    // Sync Word bytes 8
constexpr uint8_t PACKET_CONFIG_1{0x30}; // Packet mode settings
constexpr uint8_t PACKET_CONFIG_2{0x31}; // Packet mode settings
constexpr uint8_t PAYLOAD_LENGTH{0x32};  // Payload length setting
constexpr uint8_t NODE_ADRS{0x33};       // Node address
constexpr uint8_t BROADCAST_ADRS{0x34};  // Broadcast address
constexpr uint8_t FIFO_THRESH{0x35};     // Fifo threshold, Tx start condition
constexpr uint8_t SEQ_CONFIG_1{0x36};    // Top level Sequencer settings
constexpr uint8_t SEQ_CONFIG_2{0x37};    // Top level Sequencer settings
constexpr uint8_t TIMER_RESOL{0x38};     // Timer 1 and 2 resolution control
constexpr uint8_t TIMER_1_COEF{0x39};    // Timer 1 setting
constexpr uint8_t TIMER_2_COEF{0x3A};    // Timer 2 setting
constexpr uint8_t IMAGE_CAL{0x3B};       // Image calibration engine control
constexpr uint8_t TEMP{0x3C};            // Temperature Sensor value
constexpr uint8_t LOW_BAT{0x3D};         // Low Battery Indicator Settings
constexpr uint8_t IRQ_FLAGS_1{0x3E};     // Status: PLL Lock state,Timeout, RSSI
constexpr uint8_t IRQ_FLAGS_2{0x3F}; // Status: FIFO handling flags, Low Battery
constexpr uint8_t PLL_HOP{0x44};     // Control the fast frequency hopping mode
constexpr uint8_t BIT_RATE_FRAC{0x5D}; // Fractional part in the Bit Rate
                                       // division ratio
} // namespace fsk

namespace lora {
constexpr uint8_t FIFO_ADDR_PTR{0x0D};        // FIFO SPI pointer
constexpr uint8_t FIFO_TX_BASE_ADDR{0x0E};    // Start Tx data
constexpr uint8_t FIFO_RX_BASE_ADDR{0x0F};    // Start Rx data
constexpr uint8_t FIFO_RX_CURRENT_ADDR{0x10}; // Start addr last packet received
constexpr uint8_t IRQ_FLAGS_MASK{0x11};       // Mask interrupt flags
constexpr uint8_t IRQ_FLAGS{0x12};            // Interrupt flags
constexpr uint8_t RX_NB_BYTES{0x13};          // Number of received bytes
constexpr uint8_t RX_HEADER_CNT_VALUE_MSB{0x14}; // Number of valid headers MSB
constexpr uint8_t RX_HEADER_CNT_VALUE_LSB{0x15}; // Number of valid headers LSB
constexpr uint8_t RX_PACKET_CNT_VALUE_MSB{0x16}; // Number of valid packets MSB
constexpr uint8_t RX_PACKET_CNT_VALUE_LSB{0x17}; // Number of valid packets LSB
constexpr uint8_t MODEM_STAT{0x18};              // Live LoRaTM modem status
constexpr uint8_t PKT_SNR_VALUE{0x19};      // Espimation of last packet SNR
constexpr uint8_t PKT_RSSI_VALUE{0x1A};     // RSSI of last packet
constexpr uint8_t RSSI_VALUE{0x1B};         // Current RSSI
constexpr uint8_t HOP_CHANNEL{0x1C};        // FHSS start channel
constexpr uint8_t MODEM_CONFIG_1{0x1D};     // Modem PHY config 1
constexpr uint8_t MODEM_CONFIG_2{0x1E};     // Modem PHY config 2
constexpr uint8_t SYMB_TIMEOUT_LSB{0x1F};   // Receiver timeout value
constexpr uint8_t PREAMBLE_MSB{0x20};       // Size of preamble length MSB
constexpr uint8_t PREAMBLE_LSB{0x21};       // Size of Preamble length LSB
constexpr uint8_t PAYLOAD_LENGTH{0x22};     // LoRa payload length
constexpr uint8_t MAX_PAYLOAD_LENGTH{0x23}; // LoRa maximum payload length
constexpr uint8_t HOP_PERIOD{0x24};         // FHSS Hop period
constexpr uint8_t FIFO_RX_BYTE_ADDR{0x25};  // Addr of last byte written in FIFO
constexpr uint8_t MODEM_CONFIG_3{0x26};     // Modem PHY config 3
constexpr uint8_t FEI_MSB{0x28};            // Estimated frequency error
constexpr uint8_t FEI_MID{0x29};            // Estimated frequency error
constexpr uint8_t FEI_LSB{0x2A};            // Estimated frequency error
constexpr uint8_t RSSI_WIDEBAND{0x2C};      // Wideband RSSI measurement
constexpr uint8_t DETECT_OPTIMIZE{0x31};    // LoRa detection Optimize for SF6
constexpr uint8_t INVERT_IQ{0x33};          // Invert LoRa I and Q signals
constexpr uint8_t DETECTION_THRESHOLD{0x37}; // LoRa detection threshold for SF6
constexpr uint8_t SYNC_WORD{0x39};           // LoRa Sync Word

namespace mask {
constexpr uint8_t AGC{0b11111011};
constexpr uint8_t BANDWIDTH{0b00001111};
constexpr uint8_t HEADER{0b11111110};
constexpr uint8_t SPREADING_FACTOR{0b00001111};
constexpr uint8_t LOW_DATA_RATE_OPTIMIZE{0b11110111};
} // namespace mask
} // namespace lora

namespace size {
constexpr size_t DEFAULT{1};
constexpr size_t FRF{3};
constexpr size_t PREAMBLE{2};
} // namespace size

} // namespace reg
} // namespace sx127x
