
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

# --- Your executable ---
add_executable(${PROJECT_NAME} src/main.c)

# --- Include directories ---
target_include_directories(${PROJECT_NAME} PRIVATE
    external/glad/include
    external/glfw/include
    external/freetype/include
)

# --- Link libraries common to all platforms ---
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw freetype)

# --- Platform-specific OpenGL linking ---
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenGL_LIBRARY})
else() # Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()
