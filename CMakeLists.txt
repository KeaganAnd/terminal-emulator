
cmake_minimum_required(VERSION 3.0.0)
project(mag-terminal C)

set(CMAKE_C_STANDARD 11)

# --- Add GLFW (assume cloned into external/glfw) ---
add_subdirectory(external/glfw)

# --- Add GLAD ---
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# --- Add FreeType (build from source) ---
add_subdirectory(external/freetype)

# --- Build source code ---
add_subdirectory(cmake)
