#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Cube.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "UniformBufferObject.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Cube gCube;
std::vector<glm::vec3> gCubePositions = {
    glm::vec3(-0.75f, -0.75f, 0.0f),
    glm::vec3(-0.75f, 0.75f,0.0f),
    glm::vec3(0.75f, 0.75f, 0.0f),
    glm::vec3(0.75f, -0.75f, 0.0f)
};

ShaderProgram gShaderPrograms[4];
glm::mat4 gTransMat;

UniformBufferObject gUbo;
////////////////////////////////////////////////////

// GLFW callback functions
void windowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
////////////////////////////////////////////////////


static void initGlfw()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

static void createWindow()
{

    gWindow = glfwCreateWindow(WINDOW_WIDTH,
        WINDOW_HEIGHT,
        "LearnOpenGL",
        NULL,
        NULL);
    if (gWindow == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(gWindow);
}

// must be called BEFORE any OpenGL function
static void initGlad()
{
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        exit(EXIT_FAILURE);
    }
}

static void registerGlfwCallbacks()
{
    glfwSetFramebufferSizeCallback(gWindow, windowResizeCallback);
}

// called once every frame during main loop
static void draw()
{
    // clear the screen
    GLfloat r = 0.2F; // red
    GLfloat g = 0.3F; // green
    GLfloat b = 0.3F; // blue
    GLfloat a = 1.0F; // alpha
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind cube info
    glPolygonMode(GL_FRONT_AND_BACK, gCube.renderMode);
    glBindVertexArray(gCube.VAO);

    // draw each cube
    for (size_t i = 0; i < gCubePositions.size(); i++)
    {
        gShaderPrograms[i].Use();
        // create model matrix
        gTransMat = glm::mat4(1.0f);
        gTransMat = glm::translate(gTransMat, gCubePositions[i]);
        gShaderPrograms[i].SetMat4fv("uModel", gTransMat);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
    }
}

int main(void)
{
    initGlfw();
    createWindow();
    initGlad();
    registerGlfwCallbacks();

    // tell OpenGL the size and location of the rendering area
    // args: x,y,width,height
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // prevent triangles behind other triangles from being drawn
    glEnable(GL_DEPTH_TEST);

    // Cube.h
    gCube = createCube();

    // Transform.h
    gTransMat = createTransformationMatrix();

    // UniformBufferObject.h
    gUbo = createUniformBufferObject();

    // Shader.h/ShaderProgram.h
    for (size_t i = 0; i < gCubePositions.size(); i++)
    {
        gShaderPrograms[i].Create("vertexShader"+std::to_string(i+1)+".glsl", 
                                  "fragmentShader"+std::to_string(i+1)+".glsl");
        gShaderPrograms[i].SetMat4fv("uModel", gTransMat);

        // bind to uniform block buffer object index 0
        gShaderPrograms[i].SetUniformBlock("uMatrices", gUbo.bindPoint);
    }

    while (!glfwWindowShouldClose(gWindow))
    {
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gWindow, true);
        }
        // press space to switch between wired and filled drawing
        // of the rectangle
        else if (glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            static int rectRenderModes[] = {
                GL_FILL,
                GL_LINE
            };
            static size_t renderIndex = 0;
            renderIndex = !renderIndex;
            gCube.renderMode = rectRenderModes[renderIndex];
        }

        // update the common uniform buffer object data between all shaders
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                                float(WINDOW_WIDTH)/float(WINDOW_HEIGHT),
                                                0.1f,
                                                100.0f);
        //glm::mat4 projection = glm::mat4(1.0f);
        // send the projection data to the GPU
        glBindBuffer(GL_UNIFORM_BUFFER, gUbo.id);
        glBufferSubData(GL_UNIFORM_BUFFER, 
            0,                              // start addr
            sizeof(glm::mat4),              // size
            glm::value_ptr(projection));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // send the "camera" data to the GPU 
        // (set to identity here just for example since we don't have camera)
        glm::mat4 view = glm::mat4(1.0);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -6.0f));
        glBindBuffer(GL_UNIFORM_BUFFER, gUbo.id);
        glBufferSubData(GL_UNIFORM_BUFFER, 
            sizeof(glm::mat4),              // start addr
            sizeof(glm::mat4),              // size
            glm::value_ptr(view));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);

        draw();

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    //glDeleteShader(gVertexShader.id);
    //glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

