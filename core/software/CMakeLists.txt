set(SRC src/delay.cpp src/threadbase.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES common
)
