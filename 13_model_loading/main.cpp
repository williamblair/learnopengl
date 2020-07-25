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
#include "Model.h"
#include "Cube.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "Camera.h"
#include "LightSource.h"
#include "Mesh.h"


// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

LightSource gLightSource;
glm::vec3 gLightPosition = glm::vec3(1.2F, 2.0F, 1.0F);
glm::vec3 gLightDirection = glm::vec3(-0.2F, -1.0F, -0.3F);
Cube gCube;
glm::mat4 gCubeModelMat;

Model gModel;
glm::mat4 gModelModelMat;
glm::vec3 gModelPosition = glm::vec3(0.0f, 0.0f, 0.0f);

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;
std::map<std::string, int> gUniformLocations;

// shaders just used by the object representing the light
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram; 

glm::mat4 gModelTransMat;
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
    glUseProgram(gShaderProgram.id);
    glUniformMatrix4fv(gUniformLocations["uTransform"],
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(gModelTransMat)); // pointer to data
    glUniform3f(gUniformLocations["uCameraPosition"],
        gCamera.position.x,
        gCamera.position.y,
        gCamera.position.z);
    // for positional light
    glUniform3f(gUniformLocations["uPosLight.position"],
        gLightPosition.x,
        gLightPosition.y,
        gLightPosition.z);
    // for directional light
    glUniform3f(gUniformLocations["uDirLight.direction"],
        gLightDirection.x,
        gLightDirection.y,
        gLightDirection.z);
    // for spotlight
    glUniform3f(gUniformLocations["uSpotLight.position"],
        gCamera.position.x,
        gCamera.position.y,
        gCamera.position.z);
    glUniform3f(gUniformLocations["uSpotLight.direction"],
        gCamera.front.x,
        gCamera.front.y,
        gCamera.front.z);
    glUniformMatrix4fv(gUniformLocations["uModel"],
        1,
        GL_FALSE,
        glm::value_ptr(gModelModelMat));

    // main shader: model material properties
    glUniform1f(gUniformLocations["uMaterial.shininess"], 32.0F);

    // main shader: light properties
    glm::vec3 lightAmbient = glm::vec3(0.2F, 0.2F, 0.2F);
    glm::vec3 lightDiffuse = glm::vec3(0.99F, 0.99F, 0.99F);
    glUniform3fv(gUniformLocations["uDirLight.ambient"], 1, glm::value_ptr(lightAmbient));
    glUniform3fv(gUniformLocations["uDirLight.diffuse"], 1, glm::value_ptr(lightDiffuse)); // darkened
    glUniform3f(gUniformLocations["uDirLight.specular"], 1.0F, 1.0F, 1.0F);
    glUniform3fv(gUniformLocations["uPosLight.ambient"], 1, glm::value_ptr(lightAmbient));
    glUniform3fv(gUniformLocations["uPosLight.diffuse"], 1, glm::value_ptr(lightDiffuse)); // darkened
    glUniform3f(gUniformLocations["uPosLight.specular"], 1.0F, 1.0F, 1.0F);
    glUniform3fv(gUniformLocations["uSpotLight.ambient"], 1, glm::value_ptr(lightAmbient));
    glUniform3fv(gUniformLocations["uSpotLight.diffuse"], 1, glm::value_ptr(lightDiffuse)); // darkened
    glUniform3f(gUniformLocations["uSpotLight.specular"], 1.0F, 1.0F, 1.0F);
    
    // for positional light
    glUniform1f(gUniformLocations["uPosLight.constant"], 1.0F);
    glUniform1f(gUniformLocations["uPosLight.linear"], 0.22F);
    glUniform1f(gUniformLocations["uPosLight.quadratic"], 0.20F);

    // for spotlight
    glUniform1f(gUniformLocations["uSpotLight.cutoff"], glm::cos(glm::radians(12.5F)));
    glUniform1f(gUniformLocations["uSpotLight.outerCutoff"], glm::cos(glm::radians(17.5F)));

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

    // draw the model
    glUseProgram(gShaderProgram.id);
    updateTransformationMatrix(gModelTransMat, gModelPosition, gCamera);
    glUniformMatrix4fv(gUniformLocations["uTransform"],
        1, // number of matrices
        GL_FALSE, // should the matrices be transposed?
        glm::value_ptr(gModelTransMat)); // pointer to data
    gModelModelMat = glm::mat4(1.0F);
    gModelModelMat = glm::translate(gModelModelMat, gModelPosition);
    glUniformMatrix4fv(gUniformLocations["uModel"],
        1,
        GL_FALSE,
        glm::value_ptr(gModelModelMat));
    gModel.Draw(gShaderProgram);

    // draw the light source
    glUseProgram(gLightShaderProgram.id);
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

    // Cube.h
    // for the light source
    gCube = createCube();

    // Camera.h
    gCamera = createCamera();

    // Shader.h/ShaderProgram.h
    gVertexShader = createVertexShader("vertexShader.glsl");
    gFragmentShader = createFragmentShader("fragmentShader.glsl");
    gShaderProgram = createShaderProgram(gVertexShader,
        gFragmentShader);
    glUseProgram(gShaderProgram.id);

    // Model.h
    gModel.Load("backpack/backpack.obj");

    gUniformLocations["uCameraPosition"] = glGetUniformLocation(gShaderProgram.id, "uCameraPosition");
    gUniformLocations["uTransform"] = glGetUniformLocation(gShaderProgram.id, "uTransform");
    gUniformLocations["uModel"] = glGetUniformLocation(gShaderProgram.id, "uModel");
    gUniformLocations["uMaterial.shininess"] = glGetUniformLocation(gShaderProgram.id, "uMaterial.shininess");
    // for directional light and spotlight
    gUniformLocations["uDirLight.direction"] = glGetUniformLocation(gShaderProgram.id, "uDirLight.direction");
    gUniformLocations["uSpotLight.direction"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.direction");
    // for positional light and spotlight
    gUniformLocations["uPosLight.position"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.position");
    gUniformLocations["uSpotLight.position"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.position");
    // each light ambient, diffuse, specular
    gUniformLocations["uPosLight.ambient"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.ambient");
    gUniformLocations["uPosLight.diffuse"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.diffuse");
    gUniformLocations["uPosLight.specular"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.specular");
    gUniformLocations["uDirLight.ambient"] = glGetUniformLocation(gShaderProgram.id, "uDirLight.ambient");
    gUniformLocations["uDirLight.diffuse"] = glGetUniformLocation(gShaderProgram.id, "uDirLight.diffuse");
    gUniformLocations["uDirLight.specular"] = glGetUniformLocation(gShaderProgram.id, "uDirLight.specular");
    gUniformLocations["uSpotLight.ambient"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.ambient");
    gUniformLocations["uSpotLight.diffuse"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.diffuse");
    gUniformLocations["uSpotLight.specular"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.specular");
    // for positional light
    gUniformLocations["uPosLight.constant"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.constant");
    gUniformLocations["uPosLight.linear"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.linear");
    gUniformLocations["uPosLight.quadratic"] = glGetUniformLocation(gShaderProgram.id, "uPosLight.quadratic");
    // for spotlight
    gUniformLocations["uSpotLight.cutoff"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.cutoff");
    gUniformLocations["uSpotLight.outerCutoff"] = glGetUniformLocation(gShaderProgram.id, "uSpotLight.outerCutoff");

    // make a shader just for the light source, which uses a different
    // fragment shader and the same vertex shader
    gLightFragmentShader = createFragmentShader("lightFragmentShader.glsl");
    gLightShaderProgram = createShaderProgram(gVertexShader,
        gLightFragmentShader);

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gLightTransMat = createTransformationMatrix();

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

        moveCamera();

        // move the light around
        static float lightMoveRadius = 5.0F;
        gLightPosition.x = cos(glfwGetTime()) * lightMoveRadius;
        //gLightDirection.x = cos(glfwGetTime()) * lightMoveRadius;

        // Transform.h
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

