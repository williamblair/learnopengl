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

// screen size
const float SCREEN_WIDTH = 640.0f, SCREEN_HEIGHT = 480.0f;

// Vertex Array objects
GLuint vao1;

// Vertex Buffer objects
GLuint vbo1;

// Texture objects
GLuint texture;
GLuint texture2;

// shader program
GLuint program;

// matrix locations
GLuint modelMatLoc;
GLuint viewMatLoc;
GLuint projMatLoc;

// our transformation matrices
glm::mat4 modelMat;
glm::mat4 viewMat;
glm::mat4 projMat;

//-----------------------------------------------------------------------------------
//
// Init
//
void create_vao_1()
{
    static GLfloat vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 0.0f,
         0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 1.0f,
         0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 0.0f,
         0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,0.5f, 0.5f, 0.5f, 1.0f,     1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 1.0f,
         0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.5f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 0.0f,
         0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f,    0.0f, 1.0f
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

// ------------------------------------------------------------------------------------
//
// Model Matrix
//

    /* Start with an identity matrix */
    modelMat = glm::mat4(1.0f);

    /* Scale matrix */
    modelMat = glm::scale(modelMat, glm::vec3(1.0, 1.0, 1.0));

    /* Generate a translation matrix */
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 0.0f));

    /* Rotate the matrix */
    modelMat = glm::rotate(modelMat, glm::radians(45.0f), glm::vec3(1.0f, 0.0f, 0.0f));

// ------------------------------------------------------------------------------------
//
// View Matrix
//
    /* Start with an identity matrix */
    viewMat = glm::mat4(1.0f);

    /* Back the camera up some (move the world forward) */
    viewMat = glm::translate(viewMat, glm::vec3(0.0f, 0.0f, -3.0f));

// ------------------------------------------------------------------------------------
//
// Projection Matrix
//
    /* Create a persepective matrix */
    projMat = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 1000.0f);

    /* Send the matrices to the shader
     * The GL_FALSE arg is important here - if true, OpenGL will transpose/swap rows and cols
     * We use column-major ordering so no need to switch (as opposed to row major like in mathematics)
     */
    modelMatLoc = glGetUniformLocation(program, "uModelMat");
    viewMatLoc = glGetUniformLocation(program, "uViewMat");
    projMatLoc = glGetUniformLocation(program, "uProjMat");
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
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

    // we're doing 3d now so we want opengl to check who's in front of who
    glEnable(GL_DEPTH_TEST);
}

void update_matrices()
{

    /* Rotate the model just cuz */
    modelMat = glm::rotate(modelMat, glm::radians(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(modelMat));
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
}


//---------------------------------------------------------------------------------
//
// Display
//
void display(void)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    /* Clear screen and z buffer */
    glClearBufferfv(GL_COLOR, 0, black);
    glClear(GL_DEPTH_BUFFER_BIT); // will have a blank screen without this

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(vao1);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

//---------------------------------------------------------------------------------
//
// Main
//
int main(int argc, char *argv[])
{
    Window window;
    if (!window.init(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello world")) {
        return -1;
    }

    // create buffers
    init();

    while(!window.shouldClose())
    {
        update_matrices();
        display();
        window.update();
    }

    return 0;
}

