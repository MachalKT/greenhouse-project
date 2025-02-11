#include "delay.hpp"
#include "esp_log.h"

extern "C" {
void app_main(void) {

  while (1) {
    sw::delayMs(10);
  }
}
}