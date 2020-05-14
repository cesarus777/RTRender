cmake_minimum_required(VERSION 3.13.4)

project(RTRender)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})

add_library(RTRender-src OBJECT
    primitives.cpp
)


if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")  
    # using G++
    message( STATUS "GNU compiler detected, adding compile flags")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall -Wextra -Wpedantic -Werror -g")


elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # using Visual Studio C++
endif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")

add_executable(RTRender main.cpp $<TARGET_OBJECTS:RTRender-src>)
target_link_libraries(RTRender ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})
