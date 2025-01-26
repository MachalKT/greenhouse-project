set(SRC src/sht40.cpp)

set(INC inc ${PROJECT_DIR}/common)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS ${INC}
    REQUIRES software hardware
)
