set(SRC src/delay.cpp src/threadbase.cpp src/timer.cpp src/queue.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES common
)
