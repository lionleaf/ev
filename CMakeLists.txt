cmake_minimum_required(VERSION 3.10)

project(ev VERSION 0.1
           DESCRIPTION "Evolution"
           LANGUAGES C CXX)


#Main executable
add_executable(ev
    src/ev.cpp src/ev.h
    src/main.cpp src/main.h
    src/ev_math.cpp src/ev_math.h
    src/simulator.cpp src/simulator.h
    src/physics_2d.cpp src/physics_2d.h
    src/shapes.cpp src/shapes.h
    src/creatures/rolling_wheel.cpp src/creatures/rolling_wheel.h
    src/collision.cpp src/collision.h
    src/evolution.cpp src/evolution.h
    src/renderer_opengl.cpp src/renderer_opengl.h
    src/utils.cpp src/utils.h
    src/utils_opengl.cpp src/utils_opengl.h
    src/common.cpp src/common.h
    src/ev_ui.cpp src/ev_ui.h

    shaders/circle.vert shaders/circle.frag #Add shaders so they show up in Xcode
    shaders/flat.vert shaders/flat.frag

    extern/imgui-1.69/imgui_impl_glfw.cpp extern/imgui-1.69/imgui_impl_glfw.h
    extern/imgui-1.69/imgui_impl_opengl3.cpp extern/imgui-1.69/imgui_impl_opengl3.h
    )

set_target_properties(ev PROPERTIES
           CXX_STANDARD 17)

# armadillo for maths and giggles
add_subdirectory(extern/armadillo-9.3 EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE armadillo)

# imgui
add_subdirectory(extern/imgui-1.69 EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE imgui)

# glad library for OpenGL support
add_subdirectory(extern/glad EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE glad)

#glfw for making windows
add_subdirectory(extern/glfw-3.2.1 EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE glfw)

#GLM for rendering maths
add_subdirectory(extern/glm EXCLUDE_FROM_ALL)
target_link_libraries(ev PRIVATE glm_static)

#Copy shader files on every compile
add_custom_command(
        TARGET ev PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:ev>/shaders #For make and Xcode
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/shaders ${CMAKE_BINARY_DIR}/shaders #For Visual Studio
        COMMENT "Copying shaders: ev" VERBATIM
        )

# Add "Shaders" as a list of files in Visual Studio, XCode etc
source_group("Shaders" FILES REGULAR_EXPRESSION "shaders/.*\.(vert|frag)")
# Separate source list to make Visual Studio list .h and .cpp files together
source_group("extern" FILES REGULAR_EXPRESSION "(extern/imgui-1.69/.*\.(h|cpp))")
# 

# Make Visual Studio see the src/ directory structure
# Taken from https://stackoverflow.com/questions/33808087/cmake-how-to-create-visual-studio-filters

file(GLOB_RECURSE _source_list src/*.cpp* src/*.h*)
foreach(_source IN ITEMS ${_source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()
