//
// Test opengl application
//

#include "Window.h"
#include "LoadShaders.h"

#include <iostream>

// Shader program
GLuint program;

// Vertex Array objects
GLuint vao1;

// Vertex Buffer objects
GLuint vbo1;

// Element Buffer Objects
GLuint ebo1;

//-----------------------------------------------------------------------------------
//
// Init
//
void create_vao_1()
{
    static GLfloat vertices[9 + 12] = {
        // vertices     Colors
        -0.8, -0.8, 0,  1.0f, 0.0, 0.0f, 1.0f,
        0, -0.8, 0,     0.0f, 1.0f, 0.0f, 1.0f,
        -0.8, 0, 0,     0.0f, 0.0f, 1.0f, 1.0f
    };


    /* 1. Create the vertex array object to hold attribs */
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);

        /* 2. Create Buffer for data */
        glCreateBuffers(1, &vbo1);                      // array buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo1);
    
        /* 3. Store the buffer in the data */
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
        /* 4. Set data attributes */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // position attributes, 3 els per position
                7*sizeof(GLfloat),                      // each vertex has 7 float components
                (void*)0); 
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, // color attributes, 4 els per color
                7*sizeof(GLfloat),                      // each vertex has 7 float components
                (void*)(3*sizeof(GLfloat))); // skip the first 3 floats of position
        glEnableVertexAttribArray(1);

    /* 5. Stop using the attrib for now,
     * Generally this isn't really necessary */
    glBindVertexArray(0);
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

    // have opengl draw the triangles as wireframe
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}


//---------------------------------------------------------------------------------
//
// Display
//
void display(void)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glClearBufferfv(GL_COLOR, 0, black);

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

