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
#include "RoomCube.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "Camera.h"
#include "LightSource.h"
#include "Floor.h"
#include "DepthMap.h"
#include "FrameBuffer.h"
#include "ScreenTexture.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Texture gDiffuseMap;
Texture gNormalMap;
Texture gDepthMap;

Cube gCube; // for the light source to use
LightSource gLightSource;
glm::vec3 gLightPosition = glm::vec3(0.5f, 1.0f, 0.3F);
glm::vec3 gLightDirection = glm::vec3(-0.2F, 0.0F, -0.3F);
TangentPlane gTangentPlane;
glm::vec3 gTangentPlanePosition = glm::vec3(0.0F, 0.0F, 0.0F);
glm::mat4 gTangentPlaneModelMat;
std::vector<glm::vec3> gTangentPlanePositions = {
    glm::vec3(-1.0f, -2.0f, -1.0f),
    glm::vec3(2.0f, -3.5f, 0.0f)
};

Floor gFloor;
glm::vec3 gFloorPosition = glm::vec3(0.0f, -4.0f, 0.0f);
glm::mat4 gFloorModelMat;
glm::mat4 gFloorTransMat;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;

// shaders just used by the object representing the light
Shader gLightVertexShader;
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram;

glm::mat4 gTangentPlaneTransMat;
glm::mat4 gLightTransMat;
Camera gCamera;

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
    static GLuint uHeightScale = GET_LOC("uHeightScale");
    #undef GET_LOC

    glUseProgram(gLightShaderProgram.id);
    static GLuint uTransform_light = glGetUniformLocation(gLightShaderProgram.id, "uTransform");
    static GLuint uLightColor_light = glGetUniformLocation(gLightShaderProgram.id, "uLightColor");

    // draw the scene normally
    glUseProgram(gShaderProgram.id);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // clear the screen
    GLfloat r = 0.2F; // red
    GLfloat g = 0.3F; // green
    GLfloat b = 0.3F; // blue
    GLfloat a = 1.0F; // alpha
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // set positional properties
    glUniform3fv(uLightPos, 1, glm::value_ptr(gLightPosition));
    glUniform3fv(uViewPos, 1, glm::value_ptr(gCamera.position));

    // update the view and projection matrices
    glm::mat4 gProjMat = glm::perspective(glm::radians(gCamera.FOV),
        float(WINDOW_WIDTH) / float(WINDOW_HEIGHT),
        0.1F,
        100.0F);
    glm::mat4 gViewMat = glm::lookAt(gCamera.position,
        gCamera.position + gCamera.front,
        gCamera.up);
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(gProjMat));
    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(gViewMat));

#if 0
    // draw the floor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBrickWallTexture.id);
    glUniform1i(uDiffuseTex, 0); // texture 0
    // Bind our previously used depth map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gDepthMap.textureID);
    glUniform1i(uShadowMapTex, 1); // texture 1
    gFloorModelMat = glm::mat4(1.0F);
    gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(gFloorModelMat));
    glBindVertexArray(gFloor.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gFloor.numVertices);
#endif
#if 0
    // draw the room
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gBrickWallTexture.id);
    glUniform1i(uDiffuseTex, 0); // texture 0
    // Bind our previously used depth map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gDepthMap.textureID);
    glUniform1i(uShadowMapTex, 1); // texture 1
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gBrickWallNormalMap.id);
    glUniform1i(uNormalMap, 2); // texture 2
    //glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(gRoomModelMat));
    //glBindVertexArray(gRoomTangentPlane.VAO);
    //glDrawArrays(GL_TRIANGLES, 0, gRoomTangentPlane.numVertices);
#endif
    
    // set height map scale
    glUniform1f(uHeightScale, 0.3); // TODO - make this adjustable

    // draw some planes 
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDiffuseMap.id);
    glUniform1i(uDiffuseTex, 0); // texture 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormalMap.id);
    glUniform1i(uNormalMap, 1); // texture 1
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gDepthMap.id);
    glUniform1i(uDepthMap, 2); // texture 2
    for (const glm::vec3& position : gTangentPlanePositions)
    {
        gTangentPlaneModelMat = glm::mat4(1.0F);
        gTangentPlaneModelMat = glm::translate(gTangentPlaneModelMat, position);
        glUniformMatrix4fv(uModel,
            1,
            GL_FALSE,
            glm::value_ptr(gTangentPlaneModelMat));
        glBindVertexArray(gTangentPlane.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gTangentPlane.numVertices);
    }

    // draw the light source
    glUseProgram(gLightShaderProgram.id);
    glUniformMatrix4fv(uTransform_light, 1, GL_FALSE, glm::value_ptr(gLightTransMat));
    glUniform3f(uLightColor_light, 1.0f, 1.0f, 1.0f); // light color is white
    glBindVertexArray(gLightSource.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

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

    // TangentPlane.h
    gTangentPlane = createTangentPlane();
    gTangentPlaneModelMat = glm::mat4(1.0F);
    gTangentPlaneModelMat = glm::translate(gTangentPlaneModelMat, gTangentPlanePosition);

    // Cube.h
    gCube = createCube();

    // RoomTangentPlane.h
    //gRoomTangentPlane = createRoomTangentPlane();
    //gRoomModelMat = glm::mat4(1.0f); // located at origin

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

    // make a shader just for the light source
    std::cout << "Creating light shader" << std::endl;
    gLightVertexShader = createVertexShader("lightVertexShader.glsl");
    gLightFragmentShader = createFragmentShader("lightFragmentShader.glsl");
    gLightShaderProgram = createShaderProgram(gLightVertexShader,
        gLightFragmentShader);

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gTangentPlaneTransMat = createTransformationMatrix();
    gLightTransMat = createTransformationMatrix();

    // Texture.h
    gDiffuseMap = createTexture("bricks2.jpg");
    gNormalMap = createTexture("bricks2_normal.jpg");
    gDepthMap = createTexture("bricks2_disp.png");

    // DepthMap.h
    //gDepthMap = createDepthMap();

    // ScreenTexture.h
    //gScreenTexture = createScreenTexture();

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
        updateTransformationMatrix(gTangentPlaneTransMat, gTangentPlanePosition, gCamera);
        updateTransformationMatrix(gLightTransMat, gLightPosition, gCamera);

        draw();

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    glDeleteShader(gVertexShader.id);
    glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

