set(SRC src/i2c.cpp src/hrtimer.cpp src/gpio.cpp src/spi.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES driver esp_timer esp_driver_gpio common
)
