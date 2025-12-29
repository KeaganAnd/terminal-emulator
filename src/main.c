#include <stdio.h>
#include <stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <ft2build.h>
#include FT_FREETYPE_H

typedef struct {
    GLuint TextureID;
    int Width;
    int Height;
    int BearingX;
    int BearingY;
    unsigned int Advance;
} Character;

Character Characters[128];
GLuint VAO, VBO;

// Vertex shader
const char* vertexShaderSrc =
"#version 330 core\n"
"layout(location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main() {\n"
"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"    TexCoords = vertex.zw;\n"
"}\n";

// Fragment shader
const char* fragmentShaderSrc =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 FragColor;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main() {\n"
"    float alpha = texture(text, vec2(TexCoords.x, 1.0 - TexCoords.y)).r;\n"
"    FragColor = vec4(textColor, alpha);\n"
"}\n";

GLuint compileShader(GLenum type, const char* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, NULL, info);
        fprintf(stderr, "Shader compile error: %s\n", info);
    }
    return shader;
}

GLuint createShaderProgram(const char* vSrc, const char* fSrc) {
    GLuint vert = compileShader(GL_VERTEX_SHADER, vSrc);
    GLuint frag = compileShader(GL_FRAGMENT_SHADER, fSrc);
    GLuint program = glCreateProgram();
    glAttachShader(program, vert);
    glAttachShader(program, frag);
    glLinkProgram(program);
    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(program, 512, NULL, info);
        fprintf(stderr, "Program link error: %s\n", info);
    }
    glDeleteShader(vert);
    glDeleteShader(frag);
    return program;
}

// Load font and create textures
int loadFont(const char* fontPath) {
    FT_Library ft;
    FT_Face face;
    if (FT_Init_FreeType(&ft)) { fprintf(stderr,"Could not init FreeType\n"); return 0; }
    if (FT_New_Face(ft, fontPath, 0, &face)) {
        fprintf(stderr,"Failed to load font '%s'\n", fontPath);
        FT_Done_FreeType(ft);
        return 0;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c=0; c<128; c++){
        if (FT_Load_Char(face,c,FT_LOAD_RENDER | FT_LOAD_TARGET_NORMAL)) {
            fprintf(stderr, "Missing glyph for char '%c'\n", c);
            continue;
        }

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Characters[c].TextureID = tex;
        Characters[c].Width = face->glyph->bitmap.width;
        Characters[c].Height = face->glyph->bitmap.rows;
        Characters[c].BearingX = face->glyph->bitmap_left;
        Characters[c].BearingY = face->glyph->bitmap_top;
        Characters[c].Advance = face->glyph->advance.x;

        printf("[DEBUG] Loaded char '%c' (ASCII %d) size=%dx%d\n",
               c, c, Characters[c].Width, Characters[c].Height);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    return 1;
}

// Render text
void renderText(GLuint shader, const char* text, float x, float y, float scale, float r, float g, float b) {
    glUseProgram(shader);
    glUniform3f(glGetUniformLocation(shader,"textColor"), r, g, b);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    for (const char* c=text; *c; c++) {
        Character ch = Characters[(unsigned char)(*c)];
        if(ch.TextureID == 0) continue;

        float xpos = x + ch.BearingX * scale;
        float ypos = y - (ch.Height - ch.BearingY) * scale;
        float w = ch.Width * scale;
        float h = ch.Height * scale;

        float vertices[6][4] = {  
    {xpos,     ypos,     0.0f, 0.0f},
    {xpos,     ypos + h, 0.0f, 1.0f},
    {xpos + w, ypos + h, 1.0f, 1.0f},

    {xpos,     ypos,     0.0f, 0.0f},
    {xpos + w, ypos + h, 1.0f, 1.0f},
    {xpos + w, ypos,     1.0f, 0.0f}
        };

        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.Advance >> 6) * scale;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Text Debug", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr,"GLAD init failed\n"); return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint shader = createShaderProgram(vertexShaderSrc, fragmentShaderSrc);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*6*4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (!loadFont("/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf")) {
        fprintf(stderr,"Font load failed\n"); return -1;
    }

    float projection[16] = {0};
    projection[0] = 2.0f / 800.0f;
    projection[5] = 2.0f / 600.0f;
    projection[10] = -1.0f;
    projection[12] = -1.0f;
    projection[13] = -1.0f;
    projection[15] = 1.0f;

    glUseProgram(shader);
    glUniformMatrix4fv(glGetUniformLocation(shader,"projection"), 1, GL_FALSE, projection);

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Render yellow text
        renderText(shader, "Hi Sylv!", 25.0f, 550.0f, 1.0f, 1.0f, 1.0f, 0.0f);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    for (int i=0; i<128; i++) glDeleteTextures(1, &Characters[i].TextureID);
    glDeleteProgram(shader);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

