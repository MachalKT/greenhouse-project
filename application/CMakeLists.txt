set(SRC src/sensorscontroller.cpp)

set(INC inc ${PROJECT_DIR}/common)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS ${INC}
    REQUIRES components
)
