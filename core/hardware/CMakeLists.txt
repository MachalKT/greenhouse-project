set(SRC src/i2c.cpp src/hrtimer.cpp src/gpio.cpp src/spi.cpp)

set(INC inc ${PROJECT_DIR}/common)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS ${INC}
    REQUIRES driver esp_timer esp_driver_gpio
)
