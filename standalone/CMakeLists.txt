add_executable(RTRenderer app.cpp)
target_link_libraries(RTRenderer RTRender)
target_link_libraries(RTRenderer ${SDL2_LIBRATIES} ${SDL2_IMAGE_LIBRARIES})

