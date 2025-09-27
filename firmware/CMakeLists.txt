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

SET(REQUIRED_LIBS
    csv
    gd
    curl
    xapian
)

foreach(LIB IN LISTS REQUIRED_LIBS)
    find_and_require_library(${LIB})
endforeach()

###### CORE PACKAGE ######

set(CORE_SOURCES
    src/core/cover.cpp
    src/core/movie.cpp
    src/core/utils.cpp
    src/core/catalog.cpp
    src/core/sort.cpp
    src/core/img_format.cpp
    src/core/search.cpp
)

add_library(core STATIC ${CORE_SOURCES})

###### TESTS ######

enable_testing()
set(CORE_TESTS
    core/test_data
    core/test_utils 
    core/test_catalog 
    core/test_sort 
    core/test_selection
    core/test_img_format
    core/test_search
)

find_program(VALGRIND_EXECUTABLE valgrind)
foreach(T IN LISTS CORE_TESTS)
    string(REPLACE "/" "_" T_NORM ${T})
    add_executable(${T_NORM} tests/${T}.cpp)
    target_link_libraries(${T_NORM} PRIVATE core)
    string(REGEX REPLACE "/.*" "" LABEL ${T})
    add_test(NAME ${T} COMMAND ./${T_NORM})
    set_tests_properties(${T} PROPERTIES LABELS ${LABEL})
endforeach()

find_program(VALGRIND_EXECUTABLE valgrind)
foreach(T IN LISTS CORE_TESTS)
    if(VALGRIND_EXECUTABLE AND NOT T STREQUAL "core/test_search")
        string(REGEX REPLACE "/" "_" T_EXEC ${T})
        add_test(NAME valgrind_${T_EXEC}
                 COMMAND ${VALGRIND_EXECUTABLE} --leak-check=full
                 --error-exitcode=2 ./${T_EXEC})
        set_tests_properties(valgrind_${T_EXEC} PROPERTIES LABELS valgrind)
    endif()
endforeach()


###### MAIN PROGRAM #####


