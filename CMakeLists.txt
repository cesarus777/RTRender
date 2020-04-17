cmake_minimum_required(VERSION 3.13.4)

project(RTRender)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(RTRender-src OBJECT
    primitives.cpp
)

set(CMAKE_CXX_FLAGS "-std=c++17 -Wall -Wextra -Wpedantic -Werror -g")

add_executable(RTRender main.cpp $<TARGET_OBJECTS:RTRender-src>)
target_link_libraries(RTRender ${SDL2_LIBRARIES})
