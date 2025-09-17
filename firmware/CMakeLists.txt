cmake_minimum_required(VERSION 3.10)

project(featherpiflix_firmware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -O3)

include_directories(${CMAKE_SOURCE_DIR}/include)

###### CORE PACKAGE ######

set(FC_SOURCES
    src/core/cover.cpp
    src/core/movie.cpp
)

add_library(fc STATIC ${FC_SOURCES})

###### TESTS ######

add_executable(test_data tests/test_data.cpp)
target_link_libraries(test_data PRIVATE fc)

enable_testing()
add_test(NAME test_data COMMAND test_data)

###### MAIN PROGRAM #####


