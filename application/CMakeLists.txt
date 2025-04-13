# Source files
set(GREENHOUSE_CONTROLLER_SRC
    src/radiothreadcontroller.cpp
    src/timedmeter.cpp
)

set(HUB_SRC 
    src/awsiotclient.cpp
    src/awsiotthread.cpp
    src/radiothreadhub.cpp
    src/uithread.cpp
    src/wificontroller.cpp
)

# Required components
set(COMMON_REQS 
    components 
    common 
    packet
)

set(HUB_REQS 
    network 
    esp-aws-iot
)

idf_build_get_property(APP_TARGET APP_TARGET)

if(APP_TARGET STREQUAL "hub")
    set(SRC ${HUB_SRC})
    set(REQS ${COMMON_REQS} ${HUB_REQS})
elseif(APP_TARGET STREQUAL "greenhouse-controller")
    set(SRC ${GREENHOUSE_CONTROLLER_SRC})
    set(REQS ${COMMON_REQS})
else()
    message(FATAL_ERROR "APP_TARGET is undefined.")
    return()
endif()

message(STATUS "SRCS: ${SRC}")
message(STATUS "REQUIRES: ${REQS}")

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES ${REQS}
)
