set(SRC src/delay.cpp)

set(INC inc)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS ${INC}
    REQUIRES
)
