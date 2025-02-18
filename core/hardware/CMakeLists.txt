set(SRC src/i2c.cpp src/hrtimer.cpp src/gpio.cpp src/spi.cpp src/adc.cpp src/nvsstore.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES common driver esp_timer esp_driver_gpio esp_adc nvs_flash
)
