#include "delay.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ticks.hpp"

namespace sw {

void delayMs(const common::Time timeMs) { vTaskDelay(sw::msToTicks(timeMs)); }

} // namespace sw
