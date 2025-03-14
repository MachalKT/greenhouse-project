#pragma once

#include "iradio.hpp"
#include "radiopacket.hpp"
#include "threadbase.hpp"

namespace app {

class RadioThreadController final : public sw::ThreadBase {
  public:
    struct Config {
        radio::IRadio& radio;
        common::Telemetry& telemetry;
    };

    explicit RadioThreadController(Config config);

    ~RadioThreadController() = default;

  private:
    void run_() override;

    void processRadioIrqEvent_();

    void processReceiveData_();

    void handlePacketData_(const packet::radio::Type& packetType,
                           const uint8_t* buffer, const size_t bufferLength);

    void sendTelemetry_();

    static constexpr size_t MAX_READ_BUFFER{256};
    static constexpr uint32_t STACK_DEPTH{2880};
    static constexpr int PRIORITY{5};
    static constexpr sw::ThreadBase::CoreId CORE_ID{sw::ThreadBase::CoreId::_0};
    Config config_;
};

} // namespace app
