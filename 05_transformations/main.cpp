#include <iostream>
#include <cstdlib>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Rectangle.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Transform.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Rectangle gRectangle;
Texture gTextures[2];
Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;
glm::mat4 gTransMat;
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
    glClear(GL_COLOR_BUFFER_BIT);

    // draw our triangle
    glPolygonMode(GL_FRONT_AND_BACK, gRectangle.renderMode);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextures[0].id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gTextures[1].id);
    glBindVertexArray(gRectangle.VAO);
    glDrawElements(GL_TRIANGLES,
        gRectangle.numIndices,
        GL_UNSIGNED_INT,
        0);
}

static void glmVecTest()
{
    // translate vector 1,0,0 by 1,1,0
    // position vectors always have a 1.0F w component
    glm::vec4 vec(1.0F, 0.0F, 0.0F, 1.0F);
    
    std::cout << "Before translation: " << vec.x << ","
        << vec.y << ","
        << vec.z << std::endl;

    glm::mat4 transMat = glm::mat4(1.0F); // identity matrix
    transMat = glm::translate(transMat, glm::vec3(1.0F, 1.0F, 0.0F));

    vec = transMat * vec;

    std::cout << "After translation: " << vec.x << ","
        << vec.y << ","
        << vec.z << std::endl;

    // Scale and rotation
    transMat = glm::mat4(1.0F);
    transMat = glm::rotate(transMat, glm::radians(90.0F),
        glm::vec3(0.0F, 0.0F, 1.0F));
    transMat = glm::scale(transMat, glm::vec3(0.5F, 0.5F, 0.5F));

    vec = glm::vec4(1.0F, 0.0F, 0.0F, 1.0F);

    std::cout << "Before rotate/scale: " << vec.x << ","
        << vec.y << ","
        << vec.z << std::endl;

    vec = transMat * vec;

    std::cout << "After rotate/scale: " << vec.x << ","
        << vec.y << ","
        << vec.z << std::endl;
}

int main(void)
{
    initGlfw();
    createWindow();
    initGlad();
    registerGlfwCallbacks();

    glmVecTest();

    // tell OpenGL the size and location of the rendering area
    // args: x,y,width,height
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    // Rectangle.h
    gRectangle = createRectangle();

    // Shader.h/ShaderProgram.h
    gVertexShader = createVertexShader();
    gFragmentShader = createFragmentShader();
    gShaderProgram = createShaderProgram(gVertexShader,
                                        gFragmentShader);
    glUseProgram(gShaderProgram.id);

    // Texture.h
    gTextures[0] = createTexture("texture.bmp");
    gTextures[1] = createTexture("texture2.bmp");
    glUniform1i(glGetUniformLocation(gShaderProgram.id, "texSampler1"),
                                    0);
    glUniform1i(glGetUniformLocation(gShaderProgram.id, "texSampler2"),
                                    1);

    // Transform.h
    gTransMat = createTransformationMatrix();
    glUniformMatrix4fv(glGetUniformLocation(gShaderProgram.id, "uTransform"),
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(gTransMat)); // pointer to data

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
            gRectangle.renderMode = rectRenderModes[renderIndex];
        }

        // update the transformation matrix
        // Transform.h
        updateTransformationMatrix(gTransMat, (float)glfwGetTime());
        glUniformMatrix4fv(glGetUniformLocation(gShaderProgram.id, "uTransform"),
            1, // number of matrices
            GL_FALSE, // should the matrices be transposed?
            glm::value_ptr(gTransMat)); // pointer to data

        draw();

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    glDeleteShader(gVertexShader.id);
    glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

