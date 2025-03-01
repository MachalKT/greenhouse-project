set(SRC src/radiopacket.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES common
)