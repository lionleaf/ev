
add_executable(ev
    main.cpp
    renderer_opengl.h renderer_opengl.cpp 
    physics_2d.h physics_2d.cpp)

target_link_libraries(ev PUBLIC glfw)
target_link_libraries(ev PUBLIC glad)
target_include_directories(ev PUBLIC ../extern/glad/include)

add_executable(hello helloworld.cpp)
