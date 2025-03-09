#pragma once

#include "igpio.hpp"
#include "types.hpp"

namespace ui {
class Button {
  public:
    Button(hw::IGpio& ButtonPin);

    common::Error init();

    void setCallback(common::Callback cb, common::Argument argument);

    void yield();

  private:
    hw::IGpio& buttonPin_;
    common::Callback cb_;
    common::Argument arg_;
    volatile bool buttonTriggered_{false};
};
} // namespace ui
