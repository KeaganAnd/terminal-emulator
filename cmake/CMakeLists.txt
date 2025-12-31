# Source files configuration
set(SOURCES
    src/main.c
    src/shaders.c
    src/font.c
    src/renderer.c
)

set(HEADERS
    src/types.h
    src/shaders.h
    src/font.h
    src/renderer.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/external/glad/include
    ${CMAKE_SOURCE_DIR}/external/glfw/include
    ${CMAKE_SOURCE_DIR}/external/freetype/include
    ${CMAKE_SOURCE_DIR}/src
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw freetype)

# Platform-specific OpenGL linking
if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif (APPLE)
    find_library(OpenGL_LIBRARY OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenGL_LIBRARY})
else() # Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif()
