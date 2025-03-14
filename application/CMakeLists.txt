set(SRC src/timedmeter.cpp src/wificontroller.cpp src/uithread.cpp src/radiothreadcontroller.cpp src/radiothreadhub.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES components common network packet
)
