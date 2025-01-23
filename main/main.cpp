#include <stdio.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" {
void app_main(void)
{
    while(1) {
        vTaskDelay(100);
    }
}
}