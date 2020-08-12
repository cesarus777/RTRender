add_executable(RTRenderer app.cpp)

if(CMAKE_USE_PTHREADS_INIT)
    target_link_libraries(RTRenderer pthread)
endif()

target_link_libraries(RTRenderer
    ${SDL2_LIBRATIES}
    ${SDL2_IMAGE_LIBRARIES}
    RTRender
)

