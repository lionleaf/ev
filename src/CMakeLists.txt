file(GLOB PROJECT_SHADERS ../shaders/*.vert
                          ../shaders/*.frag)

file(GLOB PROJECT_SOURCES main.cpp physics_2d.cpp renderer_opengl.cpp utils.cpp utils_opengl.cpp)
file(GLOB PROJECT_HEADERS          physics_2d.h   renderer_opengl.h   utils.h   utils_opengl.h)

source_group("Shaders" FILES ${PROJECT_SHADERS})

add_executable(ev ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_SHADERS})

file(COPY ${PROJECT_SHADERS} DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(ev PUBLIC glfw)
target_link_libraries(ev PUBLIC glad)
target_include_directories(ev PUBLIC ../extern/glad/include)


add_executable(hello helloworld.cpp)
