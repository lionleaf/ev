cmake_minimum_required(VERSION 3.1...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

set(CMAKE_CXX_STANDARD 17)

project(ev VERSION 0.1
           DESCRIPTION "Evolution"
           LANGUAGES C CXX)

add_subdirectory(extern/glad)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw-3.2.1)

file(GLOB PROJECT_SHADERS shaders/*.vert
                          shaders/*.frag)

file(GLOB PROJECT_SOURCES src/main.cpp src/physics_2d.cpp src/renderer_opengl.cpp src/utils.cpp src/utils_opengl.cpp)
file(GLOB PROJECT_HEADERS          src/physics_2d.h   src/renderer_opengl.h   src/utils.h   src/utils_opengl.h)

source_group("Shaders" FILES ${PROJECT_SHADERS})

add_executable(ev ${PROJECT_SOURCES} ${PROJECT_HEADERS} ${PROJECT_SHADERS})

file(COPY ${PROJECT_SHADERS} DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(ev PUBLIC glfw)
target_link_libraries(ev PUBLIC glad)
target_include_directories(ev PUBLIC extern/glad/include)


add_executable(hello src/helloworld.cpp)
