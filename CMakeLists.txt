cmake_minimum_required(VERSION 3.13.4)

project(RTRender)



set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
include_directories(renderer/include)




if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")  
    # using Visual Studio C++
    add_compile_options(/W4 /WX)

elseif ((CMAKE_CXX_COMPILER_ID MATCHES "GNU") OR
        (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    # using G++/Clang++
    # message( STATUS "GNU compiler detected, adding compile flags")
    add_compile_options(
        -Werror
        -Wall
        -Wextra
        -Wpedantic
        #     -Wold-style-cast
        -pthread
    )
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
endif()

set(CMAKE_CXX_STANDARD 20)



add_subdirectory(renderer)
add_subdirectory(standalone)

