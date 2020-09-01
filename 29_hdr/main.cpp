#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Texture.h"
#include "Cube.h"
#include "TangentPlane.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "Camera.h"
#include "LightSource.h"
#include "Floor.h"
#include "DepthMap.h"
#include "HDRFrameBuffer.h"
#include "ScreenTexture.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Cube gCube; // The cube at the end of the tunnel
LightSource gLightSource;
std::vector<glm::vec3> gLightPositions = {
    glm::vec3( 0.0f,  0.0f, 49.5f), // back light
    glm::vec3(-1.4f, -1.9f, 9.0f),
    glm::vec3( 0.0f, -1.8f, 4.0f),
    glm::vec3( 0.8f, -1.7f, 6.0f)
};

std::vector<glm::vec3> gLightColors = {
    glm::vec3(200.0f, 200.0f, 200.0f), // super bright light to cause overload if HDR not used
    glm::vec3(0.1f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 0.2f),
    glm::vec3(0.0f, 0.1f, 0.0f)
};

Texture gWoodTexture;

Floor gFloor;
glm::vec3 gFloorPosition = glm::vec3(0.0f, -4.0f, 0.0f);
glm::mat4 gFloorModelMat;
glm::mat4 gFloorTransMat;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;

Shader gHdrVertexShader;
Shader gHdrFragmentShader;
ShaderProgram gHdrShaderProgram;

// shaders just used by the object representing the light
Shader gLightVertexShader;
Shader gLightFragmentShader;
glm::mat4 gLightTransMat;
ShaderProgram gLightShaderProgram;

Camera gCamera;

HDRFrameBuffer gHDRFrameBuffer;
ScreenTexture gScreenTexture;

float gExposure = 5.0;
bool gUseHdr = false;
////////////////////////////////////////////////////

// GLFW callback functions
void windowResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// rotate the camera FPS style
void mouseMoveCallback(GLFWwindow* window, double x, double y)
{
    static double lastX = WINDOW_WIDTH / 2.0F;
    static double lastY = WINDOW_HEIGHT / 2.0F;

    // prevent jump when mouse first enters window
    static bool firstCall = true;
    if (firstCall) {
        lastX = x;
        lastY = y;
        firstCall = false;
    }

    double offsetX = x - lastX;
    double offsetY = lastY - y; // negate this one
    lastX = x;
    lastY = y;

    static const double sensitivity = 0.1F;
    offsetX *= sensitivity;
    offsetY *= sensitivity;

    gCamera.yaw += offsetX;
    gCamera.pitch += offsetY;

    // prevent gimble lock
    if (gCamera.pitch > 89.0F) {
        gCamera.pitch = 89.0F;
    }
    if (gCamera.pitch < -89.0F) {
        gCamera.pitch = -89.0F;
    }

    // update camera pointing direction
    float dirX = cos(glm::radians(gCamera.yaw)) * cos(glm::radians(gCamera.pitch));
    float dirY = sin(glm::radians(gCamera.pitch));
    float dirZ = sin(glm::radians(gCamera.yaw)) * cos(glm::radians(gCamera.pitch));
    gCamera.front = glm::normalize(glm::vec3(dirX, dirY, dirZ));
}

// zoom in or out by changing the FOV
// difference is stored in y
void mouseScrollCallback(GLFWwindow* window, double x, double y)
{
    gCamera.FOV -= y;
    if (gCamera.FOV < 1.0F) {
        gCamera.FOV = 1.0F;
    }
    else if (gCamera.FOV > 45.0F) {
        gCamera.FOV = 45.0F;
    }
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
    glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    glfwSetCursorPosCallback(gWindow, mouseMoveCallback);
    glfwSetScrollCallback(gWindow, mouseScrollCallback);
}

static void moveCamera()
{
    // move the camera via wasd using time-based 
    // speed instead of relying on frame rate
    static float deltaTime = 0.0F;
    static float lastFrameTime = 0.0F;
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrameTime;
    lastFrameTime = currentFrame;
    float cameraSpeed = 5.0F * deltaTime;
    if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS) {
        gCamera.position += cameraSpeed * gCamera.front;
    }
    if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS) {
        gCamera.position -= cameraSpeed * gCamera.front;
    }
    if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS) {
        // calculates the camera's right vector, then use
        // it to subtract from position
        gCamera.position -= glm::normalize(
            glm::cross(gCamera.front, gCamera.up)) *
            cameraSpeed;
    }
    if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS) {
        // calculates the camera's right vector, then use
        // it to add to position
        gCamera.position += glm::normalize(
            glm::cross(gCamera.front, gCamera.up)) *
            cameraSpeed;
    }

    static bool spaceWasPressed = false;
    if (glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        if (!spaceWasPressed) {
            gUseHdr = !gUseHdr;
            std::cout << "Use HDR: " << gUseHdr << std::endl;
            spaceWasPressed = true;
        }
    } else {
        spaceWasPressed = false;
    }

    if (glfwGetKey(gWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        gExposure -= 0.1f;
        std::cout << "gExposure: " << gExposure << std::endl;
    } else if (glfwGetKey(gWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        gExposure += 0.1f;
        std::cout << "gExposure: " << gExposure << std::endl;
    }
}

// called once every frame during main loop
static void draw()
{
    // get shader uniform locations
    glUseProgram(gShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gShaderProgram.id, name) 
    static GLuint uDiffuseTex = GET_LOC("uDiffuseTex");
    static GLuint uNormalMap = GET_LOC("uNormalMap");
    static GLuint uDepthMap = GET_LOC("uDepthMap");
    static GLuint uProjection = GET_LOC("uProjection");
    static GLuint uView = GET_LOC("uView");
    static GLuint uModel = GET_LOC("uModel");
    static GLuint uLightPos = GET_LOC("uLightPos");
    static GLuint uViewPos = GET_LOC("uViewPos");
    static GLuint uInverseNormals = GET_LOC("uInverseNormals");
    #undef GET_LOC

    glUseProgram(gLightShaderProgram.id);
    static GLuint uTransform_light = glGetUniformLocation(gLightShaderProgram.id, "uTransform");
    static GLuint uLightColor_light = glGetUniformLocation(gLightShaderProgram.id, "uLightColor");

    glUseProgram(gHdrShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gHdrShaderProgram.id, name)
    static GLuint uHdrBuffer = GET_LOC("uHdrBuffer");
    static GLuint uHdr = GET_LOC("uHdr");
    static GLuint uExposure = GET_LOC("uExposure");
    #undef GET_LOC

    // render the scene into the floating point framebuffer
    glUseProgram(gShaderProgram.id);
    glBindFramebuffer(GL_FRAMEBUFFER, gHDRFrameBuffer.id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        static glm::mat4 projectionMat;
        static glm::mat4 viewMat;
        createProjectionMatrix(projectionMat, gCamera);
        createViewMatrix(viewMat, gCamera);
        glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projectionMat));
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(viewMat));
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(uDiffuseTex, 0); // GL_TEXTURE0
        glBindTexture(GL_TEXTURE_2D, gWoodTexture.id);
        // set light uniforms
        for (size_t i = 0; i < gLightPositions.size(); i++)
        {
            std::string str = "lights[" + std::to_string(i) + "].Position";
            GLuint pos = glGetUniformLocation(gShaderProgram.id, str.c_str());
            glUniform3fv(pos, 1, glm::value_ptr(gLightPositions[i]));
            str = "lights[" + std::to_string(i) + "].Color";
            pos = glGetUniformLocation(gShaderProgram.id, str.c_str());
            glUniform3fv(pos, 1, glm::value_ptr(gLightColors[i]));
        }
        glUniform3fv(uViewPos, 1, glm::value_ptr(gCamera.position));

        // render tunnel
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, 25.0f));
        modelMat = glm::scale(modelMat, glm::vec3(2.5f, 2.5f, 27.5f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniform1i(uInverseNormals, true);
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // render the floating point color buffer to the 2D quad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gHdrShaderProgram.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gHDRFrameBuffer.colorBufferID);
    glUniform1i(uHdr, gUseHdr);
    glUniform1f(uExposure, gExposure); 
    glUniform1i(uHdrBuffer, 0); // frame buffer 0
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);

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
    // tell OpenGL to always draw the pixel, ignoring the depth buffer
    // by default the function is GL_LESS 
    // (if the depth of the new pixel is less than the current pixel)
    //glDepthFunc(GL_LESS);
    // Have OpenGL auto apply gamma correction
    //glEnable(GL_FRAMEBUFFER_SRGB);

    // Cube.h
    gCube = createCube();

    // Floor.h
    gFloor = createFloor();
    gFloorModelMat = glm::mat4(1.0f);
    gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);

    // Camera.h
    gCamera = createCamera();

    // Shader.h/ShaderProgram.h
    std::cout << "Creating main shader" << std::endl;
    gVertexShader = createVertexShader("vertexShader.glsl");
    gFragmentShader = createFragmentShader("fragmentShader.glsl");
    gShaderProgram = createShaderProgram(gVertexShader,
        gFragmentShader);
    glUseProgram(gShaderProgram.id);

    gHdrVertexShader = createVertexShader("vertexShader_hdrBuffer.glsl");
    gHdrFragmentShader = createFragmentShader("fragmentShader_hdrBuffer.glsl");
    gHdrShaderProgram = createShaderProgram(gHdrVertexShader, gHdrFragmentShader);

    // make a shader just for the light source
    std::cout << "Creating light shader" << std::endl;
    gLightVertexShader = createVertexShader("lightVertexShader.glsl");
    gLightFragmentShader = createFragmentShader("lightFragmentShader.glsl");
    gLightShaderProgram = createShaderProgram(gLightVertexShader,
        gLightFragmentShader);

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gLightTransMat = createTransformationMatrix();

    // Texture.h
    gWoodTexture = createTexture("wood.png");

    // HDRFrameBuffer.h
    gHDRFrameBuffer = createHDRFrameBuffer();

    // ScreenTexture.h
    gScreenTexture = createScreenTexture();

    while (!glfwWindowShouldClose(gWindow))
    {
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gWindow, true);
        }

        moveCamera();

        // move the light around
        //static float lightMoveRadius = 5.0F;
        //gLightPosition.x = cos(glfwGetTime()) * lightMoveRadius;
        //gLightDirection.x = cos(glfwGetTime()) * lightMoveRadius;

        // Transform.h
        //updateTransformationMatrix(gLightTransMat, gLightPosition, gCamera);

        draw();

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    glDeleteShader(gVertexShader.id);
    glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

