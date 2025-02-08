#pragma once

namespace hw {
class IAdc {
    virtual float readVoltage() = 0;
};
} // namespace hw
