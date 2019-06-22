//
// Test opengl application
//

#include "Window.h"
#include "LoadShaders.h"

#include <iostream>

// Vertex Array objects
GLuint vao1;

// Vertex Buffer objects
GLuint vbo1;

//-----------------------------------------------------------------------------------
//
// Init
//
void create_vao_1()
{
    static GLfloat vertices[] = {
        -1, -1, 0,
        0, -1, 0,
        -1, 0, 0,
// draw two triangles
        0,0,0,
        1,0,0,
        1,1,0
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

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

    GLuint program = LoadShaders(shaders);
    if (program == 0) {
        std::cout << "Error loading shaders!\n";
        exit(EXIT_FAILURE);
    }
    glUseProgram(program);

    create_vao_1();
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
    //glDrawArrays(GL_TRIANGLES, 0, 3);
// draw two triangles
    glDrawArrays(GL_TRIANGLES, 0, 6);
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

