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

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Texture gDiffuseMap;
Texture gSpecularMap;
Texture gMetalTexture;

LightSource gLightSource;
glm::vec3 gLightPosition = glm::vec3(1.2F, 2.0F, 1.0F);
glm::vec3 gLightDirection = glm::vec3(-0.2F, -1.0F, -0.3F);
Cube gCube;
glm::vec3 gCubePosition = glm::vec3(0.0F, 0.0F, 0.0F);
glm::mat4 gCubeModelMat;
std::vector<glm::vec3> gCubePositions = {
    glm::vec3(-1.0f, 0.0f, -1.0f),
    glm::vec3(2.0f, 0.0f, 0.0f)
};
glm::vec3 gCubeScale = glm::vec3(1.2f, 1.2f, 1.2f); // for outline

Floor gFloor;
glm::vec3 gFloorPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 gFloorModelMat;
glm::mat4 gFloorTransMat;

ShaderProgram gShaderProgram;
ShaderProgram gOutlineShaderProgram;

// shaders just used by the object representing the light
ShaderProgram gLightShaderProgram;

glm::mat4 gCubeTransMat;
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

static void updateUniforms()
{
    // main shader
    gShaderProgram.Use();
    gShaderProgram.SetMat4fv("uTransform", gCubeTransMat);
    gShaderProgram.SetVec3fv("uCameraPosition", gCamera.position);
    // for positional light
    gShaderProgram.SetVec3fv("uPosLight.position", gLightPosition);
    // for directional light
    gShaderProgram.SetVec3fv("uDirLight.direction", gLightDirection);

    // for spotlight
    gShaderProgram.SetVec3fv("uSpotLight.position", gCamera.position);
    gShaderProgram.SetVec3fv("uSpotLight.direction", gCamera.front);
    gShaderProgram.SetMat4fv("uModel", gCubeModelMat);

    // main shader: cube material properties
    gShaderProgram.SetVec1i("uMaterial.diffuse", 0); // texture 0
    gShaderProgram.SetVec1i("uMaterial.specular", 1); // texture 1
    gShaderProgram.SetVec1f("uMaterial.shininess", 32.0f);

    // main shader: light properties
    glm::vec3 lightAmbient = glm::vec3(0.2F, 0.2F, 0.2F);
    glm::vec3 lightDiffuse = glm::vec3(0.99F, 0.99F, 0.99F);
    gShaderProgram.SetVec3fv("uDirLight.ambient", lightAmbient);
    gShaderProgram.SetVec3fv("uDirLight.diffuse", lightDiffuse);
    gShaderProgram.SetVec3fv("uDirLight.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    gShaderProgram.SetVec3fv("uPosLight.ambient", lightAmbient);
    gShaderProgram.SetVec3fv("uPosLight.diffuse", lightDiffuse);
    gShaderProgram.SetVec3fv("uPosLight.specular", glm::vec3(1.0F, 1.0F, 1.0F));
    gShaderProgram.SetVec3fv("uSpotLight.ambient", lightAmbient);
    gShaderProgram.SetVec3fv("uSpotLight.diffuse", lightDiffuse);
    gShaderProgram.SetVec3fv("uSpotLight.specular", glm::vec3(1.0F, 1.0F, 1.0F));

    // for positional light
    gShaderProgram.SetVec1f("uPosLight.constant", 1.0F);
    gShaderProgram.SetVec1f("uPosLight.linear", 0.22F);
    gShaderProgram.SetVec1f("uPosLight.quadratic", 0.20F);

    // for spotlight
    gShaderProgram.SetVec1f("uSpotLight.cutoff", glm::cos(glm::radians(12.5F)));
    gShaderProgram.SetVec1f("uSpotLight.outerCutoff", glm::cos(glm::radians(17.5F)));

    // light source shader
    gLightShaderProgram.Use();
    //static int uTransformLocation = glGetUniformLocation(gLightShaderProgram.id, "uTransform");
    // the light source uses the same vertices/buffer object
    // as the cube above
    gLightShaderProgram.SetMat4fv("uTransform", gLightTransMat);
    gLightShaderProgram.SetVec3fv("uLightColor", lightDiffuse);

    // stencil/outline shader
    gOutlineShaderProgram.Use();
    gOutlineShaderProgram.SetMat4fv("uTransform", gCubeTransMat);
    gOutlineShaderProgram.SetVec3fv("uCameraPosition", gCamera.position);
    gOutlineShaderProgram.SetMat4fv("uModel", gCubeModelMat);
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
    glEnable(GL_DEPTH_TEST);
    glStencilOp(GL_KEEP, // fail: stencil test fails; don't overwrite
        GL_KEEP, // zfail: depth test fails; don't overwrite
        GL_REPLACE); // zpass: depth test passes; overwrite
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // draw the floor
    glStencilMask(0x00); // don't write to the stencil buffer - read only
    gShaderProgram.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gMetalTexture.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gSpecularMap.id);
    updateTransformationMatrix(gFloorTransMat, gFloorPosition, gCamera);
    gShaderProgram.SetMat4fv("uTransform", gCubeTransMat);
    gFloorModelMat = glm::mat4(1.0F);
    gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);
    gShaderProgram.SetMat4fv("uModel", gFloorModelMat);
    glBindVertexArray(gFloor.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gFloor.numVertices);

    // draw the light source
    gLightShaderProgram.Use();
    glBindVertexArray(gLightSource.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);

    // draw the cube
    glStencilFunc(GL_ALWAYS, 1, 0xFF); // all fragments pass the stencil test and write 1 to the buffer
    glStencilMask(0xFF); // enable writing to the stencil buffer
    gShaderProgram.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDiffuseMap.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gSpecularMap.id);
    glPolygonMode(GL_FRONT_AND_BACK, gCube.renderMode);
    for (glm::vec3& position : gCubePositions)
    {
        updateTransformationMatrix(gCubeTransMat, position, gCamera);
        gShaderProgram.SetMat4fv("uTransform", gCubeTransMat);
        gCubeModelMat = glm::mat4(1.0F);
        gCubeModelMat = glm::translate(gCubeModelMat, position);
        gShaderProgram.SetMat4fv("uModel", gCubeModelMat);
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
    }

    // draw the 'outlines' of the cube as a slightly scaled version,
    // masked with the stencil buffer result from above
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF); // only draw if the current stencil value is not 1 (already written as 1 above)
    glStencilMask(0x00); // disable writing to the stencil buffer (read only)
    glDisable(GL_DEPTH_TEST); // always draw this outline/be on forefront of screen
    gOutlineShaderProgram.Use();
    for (glm::vec3& position : gCubePositions)
    {
        updateTransformationMatrix(gCubeTransMat, 
            position, 
            gCubeScale, 
            gCamera);
        gOutlineShaderProgram.SetMat4fv("uTransform", gCubeTransMat);
        gCubeModelMat = glm::mat4(1.0F);
        gCubeModelMat = glm::translate(gCubeModelMat, position);
        gCubeModelMat = glm::scale(gCubeModelMat, gCubeScale);
        gOutlineShaderProgram.SetMat4fv("uModel", gCubeModelMat);
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
    }

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glEnable(GL_DEPTH_TEST);
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
    //glDepthFunc(GL_ALWAYS);
    glDepthFunc(GL_LESS);

    // Enable the stencil buffer so we can record for outlining
    glEnable(GL_STENCIL_TEST);

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
    gShaderProgram.Create("vertexShader.glsl",
        "fragmentShader.glsl");
    gOutlineShaderProgram.Create("vertexShader.glsl",
        "fragmentShader_oneColor.glsl");


    // make a shader just for the light source, which uses a different
    // fragment shader and the same vertex shader
    gLightShaderProgram.Create("vertexShader.glsl", "lightFragmentShader.glsl");

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gCubeTransMat = createTransformationMatrix();
    gLightTransMat = createTransformationMatrix();

    // Texture.h
    gDiffuseMap = createTexture("marble.jpg");
    gSpecularMap = createTexture("container2_specular.png");
    gMetalTexture = createTexture("metal.png");

    while (!glfwWindowShouldClose(gWindow))
    {
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gWindow, true);
        }
        // press space to switch between ALWAYS and LESS for the
        // gl depth function
        else if (glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            static int depthFuncs[] = {
                GL_ALWAYS,
                GL_LESS
            };
            static size_t funcIndex = 0;
            funcIndex = !funcIndex;
            glDepthFunc(depthFuncs[funcIndex]);
        }

        moveCamera();

        // move the light around
        static float lightMoveRadius = 5.0F;
        gLightPosition.x = cos(glfwGetTime()) * lightMoveRadius;
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
    
    glfwTerminate();
    return 0;
}

