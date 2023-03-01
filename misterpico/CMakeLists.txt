cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(i2c_examples C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()
add_compile_options(-Werror -Wall -Wextra -Wnull-dereference)

add_subdirectory(../pico_i2c_slave/i2c_slave i2c_slave)
add_subdirectory(src)
