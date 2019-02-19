cmake_minimum_required(VERSION 3.1...3.13)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(ev VERSION 0.1
           DESCRIPTION "Evolution"
           LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw-3.2.1)

add_executable(ev src/main.cpp extern/glad/src/glad.c)
target_include_directories(ev PUBLIC extern/glad/include)
target_link_libraries(ev PUBLIC glfw)
