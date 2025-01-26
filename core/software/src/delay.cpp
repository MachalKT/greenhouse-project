#include "delay.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace core {
namespace sw {

void delayMs(const uint32_t timeMs)  {
    vTaskDelay(timeMs / portTICK_PERIOD_MS);
}

} //sw
} //core
