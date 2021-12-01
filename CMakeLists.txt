cmake_minimum_required(VERSION 3.16.0)
project(ShaderCanvas LANGUAGES C CXX)

# Include GLAD. Allow for other implementations
if (NOT DEFINED GLAD_HOME)
    set(GLAD_HOME "ext/GLAD")
endif()
include_directories("${GLAD_HOME}/include")
add_library(GLAD STATIC "${GLAD_HOME}/src/glad.c")

# Include GLFW3. Allow for other implementations
if (NOT DEFINED GLFW3_HOME)
    set(GLFW3_HOME "ext/GLFW")
endif()
set(glfw3_DIR "${GLFW3_HOME}/lib/cmake/glfw3")
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# Include STB. Allow for other implementations
if (NOT DEFINED STB_HOME)
    set(STB_HOME "ext/STB")
endif()
include_directories("${STB_HOME}/include")
add_library(STB STATIC "${STB_HOME}/stb_implementation.c")

# Add executables
add_executable(ShaderCanvas src/canvas.c)
target_link_libraries(ShaderCanvas GLAD STB glfw OpenGL::GL)