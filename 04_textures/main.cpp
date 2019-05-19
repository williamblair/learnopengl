//
// Test opengl application
//

#include "Window.h"
#include "LoadShaders.h"

// STB image loader
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>

// Vertex Array objects
GLuint vao1;

// Vertex Buffer objects
GLuint vbo1;

// Texture objects
GLuint texture;

//-----------------------------------------------------------------------------------
//
// Init
//
void create_vao_1()
{
    static GLfloat vertices[] = {
        // positions  colors                   texture coords
        -1, -1, 0,    1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        0, -1, 0,     0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 
        -1, 0, 0,     0.0f, 0.0f, 1.0f, 1.0f,  0.0f, 1.0f   
    };

    /* 1. Create the vertex array object to hold attribs */
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);

        /* 2. Create Buffer for data */
        glCreateBuffers(1, &vbo1);
        glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    
        /* 3. Store the buffer in the data */
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
        /* 4. Set data attributes */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // position attributes
                9*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, // color attributes
                9*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, // color attributes
                9*sizeof(GLfloat), (void*)(7*sizeof(GLfloat)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

    /* 5. Stop using the attrib for now,
     * Generally this isn't really necessary */
    glBindVertexArray(0);
}

void init_texture()
{
    /* 1. Get the texture data from an image */
    int width, height, nChannels;
    unsigned char *data = stbi_load("../container.jpg", &width, &height, &nChannels, 0);
    if (data == NULL) {
        std::cout << "Error loading texture\n";
        exit(EXIT_FAILURE);
    }

    /* 2. Generate an opengl texture */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    /* Set filtering parameters (optional?) */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 3. Generate the texture image from texture data */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* 4. Free data if necessary */
    stbi_image_free(data);
}

void init(void)
{
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert", 0 },
        { GL_FRAGMENT_SHADER, "triangles.frag", 0},
        { GL_NONE, NULL, 0 }
    };

    GLuint program = LoadShaders(shaders);
    if (program == 0) {
        std::cout << "Error loading shaders!\n";
        exit(EXIT_FAILURE);
    }
    glUseProgram(program);

    create_vao_1();
    init_texture();
}


//---------------------------------------------------------------------------------
//
// Display
//
void display(void)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, black);

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(vao1);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

//---------------------------------------------------------------------------------
//
// Main
//
int main(int argc, char *argv[])
{
    Window window;
    if (!window.init(640, 480, "Hello world")) {
        return -1;
    }

    // create buffers
    init();

    while(!window.shouldClose())
    {
        display();
        window.update();
    }

    return 0;
}

