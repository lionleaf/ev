cmake_minimum_required(VERSION 3.13)

project(ev VERSION 0.1
           DESCRIPTION "Evolution"
           LANGUAGES C CXX)

#Hello world toy example
add_executable(hello src/helloworld.cpp)
set_target_properties(hello PROPERTIES
           CXX_STANDARD 17)

#Main executable
add_executable(ev 
    src/main.cpp src/main.h
    src/physics_2d.cpp src/physics_2d.h
    src/renderer_opengl.cpp src/renderer_opengl.h
    src/utils.cpp src/utils.h
    src/utils_opengl.cpp src/utils_opengl.h
    shaders/circle.vert shaders/circle.frag #Add shaders so they show up in Xcode
    shaders/flat.vert shaders/flat.frag
    )
set_target_properties(ev PROPERTIES
           CXX_STANDARD 17)

# glad library for OpenGL support
add_subdirectory(extern/glad EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE glad)

#glfw for making windows
add_subdirectory(extern/glfw-3.2.1 EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE glfw)

#Copy shader files on every compile
add_custom_command(
        TARGET ev POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:ev>/shaders
        COMMENT "Copying shaders: ev" VERBATIM
        )

#Add "Shaders" as a list of files in Visual Studio, XCode etc
source_group("Shaders" FILES REGULAR_EXPRESSION "shaders/.*\.(vert|frag)")


