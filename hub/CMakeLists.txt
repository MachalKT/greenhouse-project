# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(EXTRA_COMPONENT_DIRS ../application ../common ../core ../components ../packet ../esp-aws-iot)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(hub)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES
                        CXX_STANDARD 17
                        CXX_STANDARD_REQUIRED ON)
