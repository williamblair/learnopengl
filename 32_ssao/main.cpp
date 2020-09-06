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
#include "SSAOGeometryBuffer.h"
#include "SSAOOutputBuffer.h"
#include "SSAOBlurBuffer.h"
#include "SSAOKernel.h"
#include "ScreenTexture.h"
#include "Mesh.h"
#include "Model.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Model gModel;

Cube gCube; // The cube at the end of the tunnel

// the geometry stage SSAO shader
Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;

Shader gVertexSSAOShader;
Shader gFragmentSSAOShader;
ShaderProgram gSSAOShaderProgram;

Shader gVertexSSAOBlurShader;
Shader gFragmentSSAOBlurShader;
ShaderProgram gSSAOBlurShaderProgram;

Shader gDebugBufferVertexShader;
Shader gDebugBufferFragmentShader;
ShaderProgram gDebugBufferShaderProgram;

// the final lighting pass/result of ssao output
Shader gLightVertexShader;
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram;

Camera gCamera;

SSAOGeometryBuffer gSSAOGeometryBuffer;
SSAOOutputBuffer gSSAOOutputBuffer;
SSAOBlurBuffer gSSAOBlurBuffer;
std::vector<glm::vec3> gSSAOKernel;
SSAONoiseTexture gSSAONoise;
BlurFrameBuffer gBlurFrameBuffer;
ScreenTexture gScreenTexture;

bool gUseSSAO = true;

glm::vec3 gLightPos(2.0, 4.0, -2.0);
glm::vec3 gLightColor(0.2, 0.2, 0.7);
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
            gUseSSAO = !gUseSSAO;
            std::cout << "Use SSAO: " << gUseSSAO << std::endl;
            spaceWasPressed = true;
        }
    } else {
        spaceWasPressed = false;
    }

#if 0
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
    static GLuint uProjection = GET_LOC("uProjection");
    static GLuint uView = GET_LOC("uView");
    static GLuint uModel = GET_LOC("uModel");
    static GLuint uInvertNormals = GET_LOC("uInvertNormals");
    #undef GET_LOC

    glUseProgram(gLightShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gLightShaderProgram.id, name)
    static GLuint uPositionTex_light = GET_LOC("uPositionTex");
    static GLuint uNormalTex_light = GET_LOC("uNormalTex");
    static GLuint uAlbedoTex_light = GET_LOC("uAlbedoTex");
    static GLuint uSsaoTex_light = GET_LOC("uSsaoTex");
    static GLuint uLight_Position = GET_LOC("uLight.Position");
    static GLuint uLight_Color = GET_LOC("uLight.Color");
    static GLuint uLight_Linear = GET_LOC("uLight.Linear");
    static GLuint uLight_Quadratic = GET_LOC("uLight.Quadratic");
    static GLuint uUseSSAO = GET_LOC("uUseSSAO");
    #undef GET_LOC

    glUseProgram(gSSAOShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gSSAOShaderProgram.id, name)
    static GLuint uPositionTex_ssao = GET_LOC("uPositionTex");
    static GLuint uNoiseTex_ssao = GET_LOC("uNoiseTex");
    static GLuint uNormalTex_ssao = GET_LOC("uNormalTex");
    static GLuint uProjection_ssao = GET_LOC("uProjection");
    static GLuint uSamples_ssao = GET_LOC("uSamples");
    #undef GET_LOC

    glUseProgram(gSSAOBlurShaderProgram.id);
    #define GET_LOC(name) glGetUniformLocation(gSSAOBlurShaderProgram.id, name)
    static GLuint uSsaoInput_blur = GET_LOC("uSsaoInput");
    #undef GET_LOC

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // geometry pass
    glBindFramebuffer(GL_FRAMEBUFFER, gSSAOGeometryBuffer.id);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        static glm::mat4 projectionMat;
        static glm::mat4 viewMat;
        createProjectionMatrix(projectionMat, gCamera);
        createViewMatrix(viewMat, gCamera);
        glUseProgram(gShaderProgram.id);
        glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(projectionMat));
        glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(viewMat));

        // room cube
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 7.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(7.5f, 7.5f, 7.5f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        glUniform1i(uInvertNormals, true);
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
        glUniform1i(uInvertNormals, false);

        // backpack model on the floor
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.5f, 0.0f));
        modelMat = glm::rotate(modelMat, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        modelMat = glm::scale(modelMat, glm::vec3(1.0f));
        glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
        gModel.Draw(gShaderProgram);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if 0
    // Debug draw intial buffer
    glUseProgram(gDebugBufferShaderProgram.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSSAOGeometryBuffer.colorBufferIDs[0]); // 0 = position, 1 = normal, 2 = albedo/specular
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
#endif

    // SSAO calculation
    glBindFramebuffer(GL_FRAMEBUFFER, gSSAOOutputBuffer.id);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(gSSAOShaderProgram.id);
        glUniform1i(uPositionTex_ssao, 0); // corresponds to texture 0
        glUniform1i(uNormalTex_ssao, 1); // corresponds to texture 1
        glUniform1i(uNoiseTex_ssao, 2); // corresponds to texture 2
        for (size_t i = 0; i < 64; i++)
        {
            std::string str = "uSamples[" + std::to_string(i) + "]";
            GLuint loc = glGetUniformLocation(gSSAOShaderProgram.id, str.c_str());
            glUniform3fv(loc, 1, glm::value_ptr(gSSAOKernel[i]));
        }
        glUniformMatrix4fv(uProjection_ssao, 1, GL_FALSE, glm::value_ptr(projectionMat));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gSSAOGeometryBuffer.colorBufferIDs[0]); // position
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gSSAOGeometryBuffer.colorBufferIDs[1]); // Normal
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gSSAONoise.id); // noise

        // 'draw' 2D screen-space to calculate SSAO
        glBindVertexArray(gScreenTexture.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
        
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#if 0
    // Debug draw the SSAO output 
    glUseProgram(gDebugBufferShaderProgram.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSSAOOutputBuffer.colorBufferID);
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
#endif

#if 1
    // Blur SSAO result to remove noise
    glBindFramebuffer(GL_FRAMEBUFFER, gSSAOBlurBuffer.id);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(gSSAOBlurShaderProgram.id);

        glUniform1i(uSsaoInput_blur, 0); // corresponds to texture 0

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gSSAOOutputBuffer.colorBufferID);

        // 'draw' 2D screen-space to blur the SSAO result
        glBindVertexArray(gScreenTexture.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
#endif

#if 0
    // Debug draw the blurred SSAO
    glUseProgram(gDebugBufferShaderProgram.id);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);        
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSSAOBlurBuffer.colorBufferID);
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
#endif

#if 1
    // Lighting calculation using the SSAO blurred result
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gLightShaderProgram.id);
    glm::vec3 lightPos_viewSpace = glm::vec3(viewMat * glm::vec4(gLightPos, 1.0));
    glUniform3fv(uLight_Position, 1, glm::value_ptr(lightPos_viewSpace));
    glUniform3fv(uLight_Color, 1, glm::value_ptr(gLightColor));
    glUniform1f(uLight_Linear, 0.09f);
    glUniform1f(uLight_Quadratic, 0.032f);
    glUniform1i(uPositionTex_light, 0); // corresponds to texture 0
    glUniform1i(uNormalTex_light, 1); // corresponds to texture 1
    glUniform1i(uAlbedoTex_light, 2); // corresponds to texture 2
    glUniform1i(uSsaoTex_light, 3); // corresponds to texture 3

    glUniform1i(uUseSSAO, gUseSSAO);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gSSAOGeometryBuffer.colorBufferIDs[0]); // position
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gSSAOGeometryBuffer.colorBufferIDs[1]); // normal
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gSSAOGeometryBuffer.colorBufferIDs[2]); // albedo
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gSSAOBlurBuffer.colorBufferID); // SSAO occlusion value
    glBindVertexArray(gScreenTexture.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gScreenTexture.numVertices);
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

    std::cout << "Creating SSAO shader" << std::endl;
    gVertexSSAOShader = createVertexShader("vertexShader_ssao.glsl");
    gFragmentSSAOShader = createFragmentShader("fragmentShader_ssao.glsl");
    gSSAOShaderProgram = createShaderProgram(gVertexSSAOShader, gFragmentSSAOShader);

    std::cout << "Creating SSAO blur shader" << std::endl;
    gVertexSSAOBlurShader = createVertexShader("vertexShader_ssao.glsl");
    gFragmentSSAOBlurShader = createFragmentShader("fragmentShader_ssaoBlur.glsl");
    gSSAOBlurShaderProgram = createShaderProgram(gVertexSSAOBlurShader, gFragmentSSAOBlurShader);

    // This is a different shader than the one for just drawing the light cube in prev examples,
    // this one is for the lighting pass of the SSAO process
    std::cout << "Creating light pass shader" << std::endl;
    gLightVertexShader = createVertexShader("vertexShader_ssao.glsl");
    gLightFragmentShader = createFragmentShader("fragmentShader_ssaoLighting.glsl");
    gLightShaderProgram = createShaderProgram(gLightVertexShader,
        gLightFragmentShader);

    std::cout << "Creating debug buffer shader" << std::endl;
    gDebugBufferVertexShader = createVertexShader("vertexShader_debugBuffer.glsl");
    gDebugBufferFragmentShader = createFragmentShader("fragmentShader_debugBuffer.glsl");
    gDebugBufferShaderProgram = createShaderProgram(gDebugBufferVertexShader, gDebugBufferFragmentShader);


    // SSAOGeometryBuffer.h
    gSSAOGeometryBuffer = createSSAOGeometryBuffer();

    // SSAOOutputBuffer.h
    gSSAOOutputBuffer = createSSAOOutputBuffer();

    // SSAOBlurBuffer.h
    gSSAOBlurBuffer = createSSAOBlurBuffer();

    // SSAOKernel.h
    gSSAOKernel = createSSAOKernel();
    gSSAONoise = createSSAONoise();

    // BlurFrameBuffer.h
    gBlurFrameBuffer = createBlurFrameBuffer();

    // ScreenTexture.h
    gScreenTexture = createScreenTexture();

    // Model.h
    gModel.Load("backpack/backpack.obj");

    while (!glfwWindowShouldClose(gWindow))
    {
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gWindow, true);
        }

        moveCamera();

        // move the light around
        //static float lightMoveRadius = 5.0F;
        //gLightPos.x = cos(glfwGetTime()) * lightMoveRadius;

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

