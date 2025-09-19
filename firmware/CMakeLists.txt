cmake_minimum_required(VERSION 3.10)

project(featherpiflix_firmware LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -O3 -g)

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

SET(REQUIRED_LIBS
    csv
    gd
    curl
)

foreach(LIB IN LISTS REQUIRED_LIBS)
    find_and_require_library(${LIB})
endforeach()

###### CORE PACKAGE ######

set(CORE_SOURCES
    src/core/cover.cpp
    src/core/movie.cpp
    src/core/csv.cpp
#    src/core/catalog.cpp
    src/core/sort.cpp
    src/core/img_format.cpp
)

add_library(core STATIC ${CORE_SOURCES})

###### TESTS ######

enable_testing()
set(CORE_TESTS
    data
    csv 
#    catalog 
    sort 
    selection
    img_format
)

foreach(T IN LISTS CORE_TESTS)
    add_executable(test_${T} tests/test_${T}.cpp)
    target_link_libraries(test_${T} PRIVATE core)
    add_test(NAME ${T} COMMAND test_${T})
endforeach()

###### MAIN PROGRAM #####


