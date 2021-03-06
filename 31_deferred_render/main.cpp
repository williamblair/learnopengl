#include <iostream>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <ctime>
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
#include "HDRFrameBuffer.h"
#include "BlurFrameBuffer.h"
#include "DeferredRenderBuffer.h"
#include "ScreenTexture.h"
#include "Mesh.h"
#include "Model.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Model gModel;

Cube gCube; // The cube at the end of the tunnel
LightSource gLightSource;
std::vector<glm::vec3> gLightPositions(32);
std::vector<glm::vec3> gLightColors(32);

Texture gWoodTexture;
Texture gContainerTexture;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;

Shader gDeferredVertexShader;
Shader gDeferredFragmentShader;
ShaderProgram gDeferredShaderProgram;

Shader gDebugBufferVertexShader;
Shader gDebugBufferFragmentShader;
ShaderProgram gDebugBufferShaderProgram;

// shaders just used by the object representing the light
Shader gLightVertexShader;
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram;

Camera gCamera;

DeferredRenderBuffer gDeferredFrameBuffer;
BlurFrameBuffer gBlurFrameBuffer;
ScreenTexture gScreenTexture;

float gExposure = 5.0;
bool gBloom = true;
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

#if 0
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
#endif
}

// called once every frame during main loop
static void draw()
{
    // get shader uniform locations
    glUseProgram(gShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gShaderProgram.id, name) 
    static GLuint uDiffuseTex = GET_LOC("uDiffuseTex");
    static GLuint uProjection = GET_LOC("uProjection");
    static GLuint uView = GET_LOC("uView");
    static GLuint uModel = GET_LOC("uModel");
    static GLuint uViewPos = GET_LOC("uViewPos");
    #undef GET_LOC

    glUseProgram(gDeferredShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gDeferredShaderProgram.id, name) 
    static GLuint uPositionTex = GET_LOC("uPositionTex");
    static GLuint uNormalTex = GET_LOC("uNormalTex");
    static GLuint uAlbedoSpecTex = GET_LOC("uAlbedoSpecTex");
    static GLuint uViewPos_deferred = GET_LOC("uViewPos");
    #undef GET_LOC

    glUseProgram(gLightShaderProgram.id);
    static GLuint uModel_light = glGetUniformLocation(gLightShaderProgram.id, "uModel");
    static GLuint uView_light = glGetUniformLocation(gLightShaderProgram.id, "uView");
    static GLuint uProjection_light = glGetUniformLocation(gLightShaderProgram.id, "uProjection");
    static GLuint uLightColor_light = glGetUniformLocation(gLightShaderProgram.id, "uLightColor");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render the scene into the floating point framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, gDeferredFrameBuffer.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gShaderProgram.id);
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

        glBindTexture(GL_TEXTURE_2D, gContainerTexture.id);

        // floor cube
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, -1.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(12.5f, 0.5f, 12.5f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

        // other scene cubes
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 1.5f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(2.0f, 0.0f, 1.0f));
        modelMat = glm::scale(modelMat, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(-1.0f, -1.0f, 2.0f));
        modelMat = glm::rotate(modelMat, glm::radians(60.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 2.7f, 4.0f));
        modelMat = glm::rotate(modelMat, glm::radians(23.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(-2.0f, 1.0f, -3.0f));
        modelMat = glm::rotate(modelMat, glm::radians(127.0f), glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
        
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(-3.0f, 0.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(0.5f, 0.5f, 0.5f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

        // Model
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(-2.0f, 1.0f, 3.0f));
        modelMat = glm::scale(modelMat, glm::vec3(0.25f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        gModel.Draw(gShaderProgram);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#if 0
    // Debug draw intial buffer
    glUseProgram(gDebugBufferShaderProgram.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDeferredFrameBuffer.colorBufferIDs[2]);
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
#endif

#if 1
    glUseProgram(gDeferredShaderProgram.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDeferredFrameBuffer.colorBufferIDs[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gDeferredFrameBuffer.colorBufferIDs[1]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gDeferredFrameBuffer.colorBufferIDs[2]);
    glUniform1i(uPositionTex, 0);
    glUniform1i(uNormalTex, 1);
    glUniform1i(uAlbedoSpecTex, 2);
    glUniform3fv(uViewPos_deferred, 1, glm::value_ptr(gCamera.position));
    for (size_t i = 0; i < 32; i++)
    {
        std::string locString = "lights[" + std::to_string(i) + "].Position";
        GLuint loc = glGetUniformLocation(gDeferredShaderProgram.id, locString.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(gLightPositions[i]));
        locString = "lights[" + std::to_string(i) + "].Color";
        loc = glGetUniformLocation(gDeferredShaderProgram.id, locString.c_str());
        glUniform3fv(loc, 1, glm::value_ptr(gLightColors[i]));
    }
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);

    // copy the geometry depth buffer from the first pass so we can
    // use it for depth testing when we draw non-deferred
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gDeferredFrameBuffer.id);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // bind to default
    glBlitFramebuffer(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(gLightShaderProgram.id);
    glUniformMatrix4fv(uProjection_light, 1, GL_FALSE, glm::value_ptr(projectionMat));
    glUniformMatrix4fv(uView_light, 1, GL_FALSE, glm::value_ptr(viewMat));

    for (size_t i = 0; i < gLightPositions.size(); i++)
    {
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(gLightPositions[i]));
        modelMat = glm::scale(modelMat, glm::vec3(0.25f));
        glUniformMatrix4fv(uModel_light, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniform3fv(uLightColor_light, 1, glm::value_ptr(gLightColors[i]));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
    }
#endif
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

    // Camera.h
    gCamera = createCamera();

    // Shader.h/ShaderProgram.h
    std::cout << "Creating main shader" << std::endl;
    gVertexShader = createVertexShader("vertexShader.glsl");
    gFragmentShader = createFragmentShader("fragmentShader.glsl");
    gShaderProgram = createShaderProgram(gVertexShader,
        gFragmentShader);
    glUseProgram(gShaderProgram.id);

    std::cout << "Creating deferred shader" << std::endl;
    gDeferredVertexShader = createVertexShader("vertexShader_deferred.glsl");
    gDeferredFragmentShader = createFragmentShader("fragmentShader_deferred.glsl");
    gDeferredShaderProgram = createShaderProgram(gDeferredVertexShader, gDeferredFragmentShader);

    // make a shader just for the light source
    std::cout << "Creating light shader" << std::endl;
    gLightVertexShader = createVertexShader("vertexShader_lightSource.glsl");
    gLightFragmentShader = createFragmentShader("fragmentShader_lightSource.glsl");
    gLightShaderProgram = createShaderProgram(gLightVertexShader,
        gLightFragmentShader);

    std::cout << "Creating debug buffer shader" << std::endl;
    gDebugBufferVertexShader = createVertexShader("vertexShader_debugBuffer.glsl");
    gDebugBufferFragmentShader = createFragmentShader("fragmentShader_debugBuffer.glsl");
    gDebugBufferShaderProgram = createShaderProgram(gDebugBufferVertexShader, gDebugBufferFragmentShader);

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    //gLightTransMat = createTransformationMatrix();

    // Texture.h
    gWoodTexture = createTexture("wood.png");
    gContainerTexture = createTexture("container2.png");

    // DeferredRenderBuffer.h
    gDeferredFrameBuffer = createDeferredRenderBuffer();

    // BlurFrameBuffer.h
    gBlurFrameBuffer = createBlurFrameBuffer();

    // ScreenTexture.h
    gScreenTexture = createScreenTexture();

    // Model.h
    gModel.Load("backpack/backpack.obj");

    // initialize lights
    srand(time(0));
    for (size_t i = 0; i < 32; i++)
    {
        gLightPositions[i] = glm::vec3(float(rand() % 100) / 100.0 * 6.0 - 3.0,
                                       float(rand() % 100) / 100.0 * 6.0 - 4.0, 
                                       float(rand() % 100) / 100.0 * 6.0 - 3.0);
        gLightColors[i] = glm::vec3(float(rand() % 100) / 200.0 + 0.5, 
                                    float(rand() % 100) / 200.0 + 0.5, 
                                    float(rand() % 100) / 200.0 + 0.5);

        std::cout << "Light Pos: " << gLightPositions[i].x << " " << gLightPositions[i].y << " " << gLightPositions[i].z << std::endl;
        std::cout << "Light Color: " << gLightColors[i].x << " " << gLightColors[i].y << " " << gLightColors[i].z << std::endl;
    }

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

