set(SRC src/sht40.cpp)

idf_component_register(
    SRCS ${SRC}
    INCLUDE_DIRS inc
    REQUIRES software hardware common
)
