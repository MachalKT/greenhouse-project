set(SRC src/timedmeter.cpp src/wificontroller.cpp src/radiothread.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES components common network packet
)
