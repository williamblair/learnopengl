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

// Uniform locations
GLuint uColorLoc;

// color to send to the uniform
GLuint color_index = 0;
GLfloat colors[3][4] = {
    {1.0f, 0.0f, 0.0f, 1.0f}, // Red
    {0.0f, 1.0f, 0.0f, 1.0f}, // green
    {0.0f, 0.0f, 1.0f, 1.0f}  // blue
};

// to be called when a key is pressed
void keyHandler(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // increase the color index
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
        color_index = (color_index + 1) % 3;

    // send the new color to the shader
    glUniform4fv(uColorLoc, 1, colors[color_index]);
}

//-----------------------------------------------------------------------------------
//
// Init
//
void create_vao_1()
{
    static GLfloat vertices[12] = {
        -0.8, -0.8, 0,
        0, -0.8, 0,
        -0.8, 0,
        0, 0, 0
    };

    static GLushort indices[6] = {
        0, 1, 2,
        1, 2, 3
    };

    /* 1. Create the vertex array object to hold attribs */
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);

        /* 2. Create Buffer for data */
        glCreateBuffers(1, &vbo1);                      // array buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo1);
        glCreateBuffers(1, &ebo1);                      // element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo1);
    
        /* 3. Store the buffer in the data */
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
        /* 4. Set data attributes */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        glEnableVertexAttribArray(0);

    /* 5. Stop using the attrib for now,
     * Generally this isn't really necessary */
    glBindVertexArray(0);
}

void init_color_uniform()
{
    /* 1. Get the uniform's location (done ONLY once) */
    uColorLoc = glGetUniformLocation(program, "uColor");

    /* 2. Send it data */
    glUniform4fv(uColorLoc, 1, colors[color_index]);
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

    init_color_uniform();

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
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
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

    std::cout << "Press space to change color!\n";
    window.setKeyHandler(keyHandler);

    // create buffers
    init();

    while(!window.shouldClose())
    {
        display();
        window.update();
    }

    return 0;
}

