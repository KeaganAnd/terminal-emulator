
cmake_minimum_required(VERSION 3.10)
project(mag-terminal C)

set(CMAKE_C_STANDARD 11)

# --- Add GLFW (assume cloned into external/glfw) ---
add_subdirectory(external/glfw)

# --- Add GLAD ---
add_library(glad external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)

# --- Add FreeType (build from source) ---
add_subdirectory(external/freetype)

# --- Source files ---
set(SOURCES
	src/main.c
	src/shaders.c
	src/font.c
	src/renderer.c
    src/textHandler.c
    src/shell.c
	src/terminal_logic.c
	src/input.c
)

set(HEADERS
	src/types.h
	src/shaders.h
	src/font.h
	src/renderer.h
    src/textHandler.h
    src/globals.h
    src/shell.h
	src/terminal_logic.h
	src/input.h
)

# --- Build executable ---
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# --- Include directories ---
target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/external/glad/include
	${CMAKE_SOURCE_DIR}/external/glfw/include
	${CMAKE_SOURCE_DIR}/external/freetype/include
	${CMAKE_SOURCE_DIR}/src
)

# --- Link libraries ---
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw freetype)

# --- Platform-specific OpenGL linking ---
if (WIN32)
	target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif (APPLE)
	find_library(OpenGL_LIBRARY OpenGL)
	target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenGL_LIBRARY})
else()
	target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()
