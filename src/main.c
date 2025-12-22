
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

int main(void) {
    // Initialize GLFW
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    // Set OpenGL version to 3.3 Core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // Required on macOS
#endif

    // Create window
    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return 1;
    }
    
    // Setting up freetype and font
    FT_Library library;
    FT_Face face;
    
    FT_Error error = FT_Init_FreeType( &library );
    if (error) {
                printf("Freetype failed to initialize");
        }
   // If you want to make cross platform need to find files in different places 
    error = FT_New_Face(library, "C:/Windows/Fonts/arial.ttf", 0, &face);

    if(error == FT_Err_Unknown_File_Format){
        printf("The font file is opened nd read but not supported");
    } else if (error){
        printf("An error has occured loading the font %d", error);
    }

    FT_Set_Pixel_Sizes(face, 0, 40);
    
    typedef struct {
        float x;
        float y;
    } vec2;

    typedef struct {
        GLuint textureID;
        vec2 size;
        vec2 bearing;
        unsigned int advance;
    } Character; 

    Character Characters[128];

    // Loading and storing the glyphs into textures for opengl
    for(unsigned char c = 0; c < 128; c++) {
       if (FT_Load_Char(face, c, FT_LOAD_RENDER)) continue;
        
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                     face ->glyph ->bitmap.width,
                     face ->glyph ->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE,
                     face ->glyph ->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Characters[c].textureID = texture;
        Characters[c].size = (vec2){face ->glyph ->bitmap.width, face ->glyph ->bitmap.rows};
        Characters[c].bearing = (vec2){face -> glyph ->bitmap_left, face -> glyph ->bitmap_top};
        Characters[c].advance = face ->glyph ->advance.x;
    }

    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Set background color
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
