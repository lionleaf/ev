
add_executable(ev main.cpp renderer_opengl.cpp renderer_opengl.h)
target_include_directories(ev PUBLIC ../extern/glad/include)
target_link_libraries(ev PUBLIC glfw)

add_executable(hello helloworld.cpp)
