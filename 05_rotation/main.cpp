//
// Test opengl application
//

#include "Window.h"
#include "LoadShaders.h"

// matrix library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
GLuint texture2;

// shader program
GLuint program;

// transformation matrix location
GLuint matloc;

// our transformation matrix
glm::mat4 trans;

//-----------------------------------------------------------------------------------
//
// Init
//
void create_vao_1()
{
    static GLfloat vertices[] = {
        // positions  colors                   texture coords
        -0.5, -0.5, 0,    1.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f,
        0.5, -0.5, 0,     0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 
        0, 0.5, 0,     0.0f, 0.0f, 1.0f, 1.0f,  0.5f, 1.0f   
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

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE0 */
    glUniform1i(glGetUniformLocation(program, "myTexture"), 0);
}

void init_texture2()
{
    /* 1. Get the texture data from an image */
    int width, height, nChannels;
    unsigned char *data = stbi_load("../awesomeface.png", &width, &height, &nChannels, 0);
    if (data == NULL) {
        std::cout << "Error loading texture\n";
        exit(EXIT_FAILURE);
    }

    /* 2. Generate an opengl texture */
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    /* Set filtering parameters (optional?) */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 3. Generate the texture image from texture data 
     * This image is a png so it has alpha values as well */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* 4. Free data if necessary */
    stbi_image_free(data);

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE1 */
    GLuint tex2loc = glGetUniformLocation(program, "myTexture2");
    glUniform1i(tex2loc, 1);
}

void init_matuniform()
{
    /* Note the order is reverse since operations are applied right to left */

    /* Start with an identity matrix */
    trans = glm::mat4(1.0f);

    /* Scale matrix */
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    /* Generate a translation matrix */
    trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));

    /* Rotate the matrix */
    trans = glm::rotate(trans, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    /* Send the matrix to the shader
     * The GL_FALSE arg is important here - if true, OpenGL will transpose/swap rows and cols
     * We use column-major ordering so no need to switch (as opposed to row major like in mathematics)
     */
    matloc = glGetUniformLocation(program, "uTransMat");
    glUniformMatrix4fv(matloc, 1, GL_FALSE, glm::value_ptr(trans));
}

void init(void)
{
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert", 0 },
        { GL_FRAGMENT_SHADER, "triangles.frag", 0},
        { GL_NONE, NULL, 0 }
    };

    program = LoadShaders(shaders);
    if (program == 0) {
        std::cout << "Error loading shaders!\n";
        exit(EXIT_FAILURE);
    }
    glUseProgram(program);

    create_vao_1();
    init_texture();
    init_texture2();
    init_matuniform();
}

void rotate_tri()
{
    /* The amount to increase the rotation and the axis to rotate it around */
    static float angle = 1.0f;
    static glm::vec3 rotvec(0.0f, 1.0f, 0.5f);


// we don't do this here as we're rotating from the existing/current matrix, not reconstructing it each frame
#if 0
    /* Scale matrix */
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    /* Generate a translation matrix */
    trans = glm::translate(trans, glm::vec3(0.5f, 0.5f, 0.0f));
#endif
    /* Rotate the matrix */
    trans = glm::rotate(trans, glm::radians(angle), rotvec);
    
    /* Send the matrix to the shader */
    glUniformMatrix4fv(matloc, 1, GL_FALSE, glm::value_ptr(trans));

}

//---------------------------------------------------------------------------------
//
// Display
//
void display(void)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, black);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

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
        rotate_tri();

        display();
        window.update();
    }

    return 0;
}

