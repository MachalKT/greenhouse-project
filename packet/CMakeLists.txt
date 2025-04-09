set(SRC src/radiopacket.cpp src/awspacket.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES common json
)
