cmake_minimum_required(VERSION 3.10)

project(featherpiflix_firmware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -O3)

include_directories(${CMAKE_SOURCE_DIR}/include)

##### LIBRARIES #####

macro(find_and_require_library LIB_NAME)
    find_library(${LIB_NAME}_LIBRARY NAMES ${LIB_NAME})
    if(${LIB_NAME}_LIBRARY)
        message(STATUS "lib${LIB_NAME} found : ${${LIB_NAME}_LIBRARY}")
    else()
        message(FATAL_ERROR "Library lib${LIB_NAME} not found!")
    endif()
    link_libraries(${LIB_NAME})
endmacro()

find_and_require_library(csv)

###### CORE PACKAGE ######

set(FC_SOURCES
    src/core/cover.cpp
    src/core/movie.cpp
    src/core/csv.cpp
#    src/core/catalog.cpp
)

add_library(fc STATIC ${FC_SOURCES})

###### TESTS ######

add_executable(test_data tests/test_data.cpp)
target_link_libraries(test_data PRIVATE fc)

add_executable(test_csv tests/test_csv.cpp)
target_link_libraries(test_csv PRIVATE fc)

#add_executable(test_catalog tests/test_catalog.cpp)
#target_link_libraries(test_catalog PRIVATE fc)

enable_testing()
add_test(NAME csv COMMAND test_csv)
add_test(NAME data COMMAND test_data)
#add_test(NAME catalog COMMAND test_catalog)

###### MAIN PROGRAM #####


