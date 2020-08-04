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
Texture gWoodTexture;

LightSource gLightSource;
glm::vec3 gLightPosition = glm::vec3(-2.0f, 4.0f, -1.0F);
glm::vec3 gLightDirection = glm::vec3(-0.2F, -1.0F, -0.3F);
Cube gCube;
glm::vec3 gCubePosition = glm::vec3(0.0F, 0.0F, 0.0F);
glm::mat4 gCubeModelMat;
std::vector<glm::vec3> gCubePositions = {
    glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(2.0f, 0.0f, 0.0f)
};

Floor gFloor;
glm::vec3 gFloorPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 gFloorModelMat;
glm::mat4 gFloorTransMat;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;
std::map<std::string, int> gUniformLocations;

// shaders just used by the object representing the light
Shader gLightVertexShader;
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram;

// used to draw the framebuffered screen texture
Shader gScreenVertexShader;
Shader gScreenFragmentShader;
ShaderProgram gScreenShaderProgram;
ScreenTexture gScreenTexture;

glm::mat4 gCubeTransMat;
glm::mat4 gLightTransMat;
Camera gCamera;

Shader gDepthVertexShader;
Shader gDepthFragmentShader;
ShaderProgram gDepthShaderProgram;
DepthMap gDepthMap;
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

static void updateUniforms()
{
    // main shader
    glUseProgram(gShaderProgram.id);
    glUniform3f(gUniformLocations["uViewPos"],
        gCamera.position.x,
        gCamera.position.y,
        gCamera.position.z);
    // for positional light
    glUniform3f(gUniformLocations["uLightPos"],
        gLightPosition.x,
        gLightPosition.y,
        gLightPosition.z);

    // main shader: light properties
    glm::vec3 lightAmbient = glm::vec3(0.2F, 0.2F, 0.2F);
    glm::vec3 lightDiffuse = glm::vec3(0.99F, 0.99F, 0.99F);
    // light source shader
    glUseProgram(gLightShaderProgram.id);
    static int uTransformLocation = glGetUniformLocation(gLightShaderProgram.id, "uTransform");
    // the light source uses the same vertices/buffer object
    // as the cube above
    glUniformMatrix4fv(uTransformLocation,
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(gLightTransMat)); // pointer to data
    static int uLightColorLocation = glGetUniformLocation(gLightShaderProgram.id, "uLightColor");
    glUniform3fv(uLightColorLocation, 1, glm::value_ptr(lightDiffuse));

    // debug depth shader
    static int uDepthTextureLoc = glGetUniformLocation(gScreenShaderProgram.id, "uScreenTexture");
    glUniform1i(uDepthTextureLoc, 0); // GL texture 2D 0
}

// called once every frame during main loop
static void draw()
{
    // draw to the depth map
    // clear the screen
    GLfloat r = 0.2F; // red
    GLfloat g = 0.3F; // green
    GLfloat b = 0.3F; // blue
    GLfloat a = 1.0F; // alpha
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gDepthShaderProgram.id);
    glViewport(0, 0, gDepthMap.width, gDepthMap.height);
    glBindFramebuffer(GL_FRAMEBUFFER, gDepthMap.framebufferID);
    glClear(GL_DEPTH_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0);
    glCullFace(GL_FRONT); 

        // calculate light matrices
        // (this is a directional light)
        float nearPlane = 1.0f;
        float farPlane = 7.5f;
        glm::mat4 lightProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,  
                                            nearPlane, farPlane);
        // light 'position', although this is a directional light
        // looking at center of world
        // up direction
        glm::mat4 lightViewMat = glm::lookAt(gLightPosition, 
                                             glm::vec3(0.0f, 0.0f, 0.0f), 
                                             glm::vec3(0.0f, 1.0f, 0.0f)); 
        glm::mat4 lightSpaceMat = lightProjMat * lightViewMat;

        static GLuint depthTransformLoc = 
            glGetUniformLocation(gDepthShaderProgram.id, "uTransform");
        glUniformMatrix4fv(depthTransformLoc,
                            1,
                            GL_FALSE,
                            glm::value_ptr(lightSpaceMat));
    
        // draw the floor
        gFloorModelMat = glm::mat4(1.0F);
        gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);
        static GLuint depthShaderModelLoc = 
            glGetUniformLocation(gDepthShaderProgram.id, "uModel");
        glUniformMatrix4fv(depthShaderModelLoc,
            1,
            GL_FALSE,
            glm::value_ptr(gFloorModelMat));
        glBindVertexArray(gFloor.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gFloor.numVertices);

        // draw some cubes
        for (const glm::vec3& position : gCubePositions)
        {
            gCubeModelMat = glm::mat4(1.0F);
            gCubeModelMat = glm::translate(gCubeModelMat, position);
            glUniformMatrix4fv(depthShaderModelLoc,
                1,
                GL_FALSE,
                glm::value_ptr(gCubeModelMat));
            glBindVertexArray(gCube.VAO);
            glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
        }
    
        // draw the light source
        //glUseProgram(gLightShaderProgram.id);
        //glBindVertexArray(gLightSource.VAO);
        //glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

    // done drawing to the depth map
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);

#if 1
    // draw the scene normally
    glUseProgram(gShaderProgram.id);
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // clear the screen
    r = 0.2F; // red
    g = 0.3F; // green
    b = 0.3F; // blue
    a = 1.0F; // alpha
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // update the view and projection matrices
    glm::mat4 gProjMat = glm::perspective(glm::radians(gCamera.FOV),
        float(WINDOW_WIDTH) / float(WINDOW_HEIGHT),
        0.1F,
        100.0F);
    glm::mat4 gViewMat = glm::lookAt(gCamera.position,
        gCamera.position + gCamera.front,
        gCamera.up);
    glUniformMatrix4fv(gUniformLocations["uProj"],
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(gProjMat)); // pointer to data
    glUniformMatrix4fv(gUniformLocations["uView"],
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(gViewMat)); // pointer to data
    // calculated above
    glUniformMatrix4fv(gUniformLocations["uLightSpaceMat"],
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(lightSpaceMat)); // pointer to data

    // draw the floor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWoodTexture.id);
    glUniform1i(gUniformLocations["uDiffuseTex"], 0); // texture 0
    // Bind our previously used depth map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gDepthMap.textureID);
    glUniform1i(gUniformLocations["uShadowMapTex"], 1); // texture 1
    gFloorModelMat = glm::mat4(1.0F);
    gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);
    glUniformMatrix4fv(gUniformLocations["uModel"],
        1,
        GL_FALSE,
        glm::value_ptr(gFloorModelMat));
    glBindVertexArray(gFloor.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gFloor.numVertices);

    // draw some cubes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWoodTexture.id);
    glUniform1i(gUniformLocations["uDiffuseTex"], 0); // texture 0
    for (const glm::vec3& position : gCubePositions)
    {
        gCubeModelMat = glm::mat4(1.0F);
        gCubeModelMat = glm::translate(gCubeModelMat, position);
        glUniformMatrix4fv(gUniformLocations["uModel"],
            1,
            GL_FALSE,
            glm::value_ptr(gCubeModelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
    }

    // draw the light source
    glUseProgram(gLightShaderProgram.id);
    glBindVertexArray(gLightSource.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

#endif
    // draw the depth buffer texture (for debug)
#if 0
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // clear the screen
    r = 0.2F; // red
    g = 0.3F; // green
    b = 0.3F; // blue
    a = 1.0F; // alpha
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(gScreenShaderProgram.id);
    glBindVertexArray(gScreenTexture.VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDepthMap.textureID);
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
    glDepthFunc(GL_LESS);
    // Have OpenGL auto apply gamma correction
    glEnable(GL_FRAMEBUFFER_SRGB);

    // Cube.h
    gCube = createCube();
    gCubeModelMat = glm::mat4(1.0F);
    gCubeModelMat = glm::translate(gCubeModelMat, gCubePosition);

    // Floor.h
    gFloor = createFloor();
    gFloorModelMat = glm::mat4(1.0f);
    gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);

    // Camera.h
    gCamera = createCamera();

    // Shader.h/ShaderProgram.h
    gVertexShader = createVertexShader("vertexShader.glsl");
    gFragmentShader = createFragmentShader("fragmentShader.glsl");
    gShaderProgram = createShaderProgram(gVertexShader,
        gFragmentShader);
    glUseProgram(gShaderProgram.id);

    // Frame buffer drawn to screen shader
    gScreenVertexShader = createVertexShader("vertexShader_screenTexture.glsl");
    gScreenFragmentShader = createFragmentShader("fragmentShader_screenTexture.glsl");
    gScreenShaderProgram = createShaderProgram(gScreenVertexShader, gScreenFragmentShader);

    gDepthVertexShader = createVertexShader("vertexShader_lightSpace.glsl");
    gDepthFragmentShader = createFragmentShader("fragmentShader_lightSpace.glsl");
    gDepthShaderProgram = createShaderProgram(gDepthVertexShader, 
        gDepthFragmentShader);

    gUniformLocations["uDiffuseTex"] = glGetUniformLocation(gShaderProgram.id, "uDiffuseTex");
    gUniformLocations["uShadowMapTex"] = glGetUniformLocation(gShaderProgram.id, "uShadowMapTex");
    gUniformLocations["uLightPos"] = glGetUniformLocation(gShaderProgram.id, "uLightPos");
    gUniformLocations["uLightPos"] = glGetUniformLocation(gShaderProgram.id, "uLightPos");
    gUniformLocations["uProj"] = glGetUniformLocation(gShaderProgram.id, "uProj");
    gUniformLocations["uView"] = glGetUniformLocation(gShaderProgram.id, "uView");
    gUniformLocations["uModel"] = glGetUniformLocation(gShaderProgram.id, "uModel");
    gUniformLocations["uLightSpaceMat"] = glGetUniformLocation(gShaderProgram.id, "uLightSpaceMat");

    // make a shader just for the light source
    gLightVertexShader = createVertexShader("lightVertexShader.glsl");
    gLightFragmentShader = createFragmentShader("lightFragmentShader.glsl");
    gLightShaderProgram = createShaderProgram(gLightVertexShader,
        gLightFragmentShader);

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gCubeTransMat = createTransformationMatrix();
    gLightTransMat = createTransformationMatrix();

    // Texture.h
    gDiffuseMap = createTexture("marble.jpg");
    gWoodTexture = createTexture("wood.png");

    // DepthMap.h
    gDepthMap = createDepthMap();

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
        updateTransformationMatrix(gCubeTransMat, gCubePosition, gCamera);
        updateTransformationMatrix(gLightTransMat, gLightPosition, gCamera);

        // send updated matrix/position data to the shaders
        updateUniforms();

        draw();

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    glDeleteShader(gVertexShader.id);
    glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

