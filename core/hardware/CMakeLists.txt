set(SRC src/i2c.cpp)

set(INC inc ${PROJECT_DIR}/common)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS ${INC}
    REQUIRES driver
)
