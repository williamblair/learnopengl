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

const size_t amount = 100000;
float radius = 50.0F;
float offset = 2.5F;
Model gAsteroidModel;
glm::mat4 gAsteroidModelMats[amount];

Model gPlanetModel;
glm::mat4 gPlanetModelMat;

//Shader gVertexShader;
//Shader gFragmentShader;
ShaderProgram gShaderProgram;
std::map<std::string, int> gUniformLocations;

// shaders just used by the object representing the light
//Shader gLightFragmentShader;
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
    gShaderProgram.Use();
    //gShaderProgram.SetMat4fv("uTransform", gModelTransMat);
    gShaderProgram.SetVec3fv("uCameraPosition", gCamera.position);
    // for positional light
    gShaderProgram.SetVec3fv("uPosLight.position", gLightPosition);
    // for directional light
    gShaderProgram.SetVec3fv("uDirLight.direction", gLightDirection);
    // for spotlight
    gShaderProgram.SetVec3fv("uSpotLight.position", gCamera.position);
    gShaderProgram.SetVec3fv("uSpotLight.direction", gCamera.front);
    gShaderProgram.SetMat4fv("uModel", gModelModelMat);

    // main shader: model material properties
    gShaderProgram.SetVec1f("uMaterial.shininess", 32.0F);

    // main shader: light properties
    glm::vec3 lightAmbient = glm::vec3(0.2F, 0.2F, 0.2F);
    glm::vec3 lightDiffuse = glm::vec3(0.99F, 0.99F, 0.99F);
    glm::vec3 onesVec = glm::vec3(1.0F, 1.0F, 1.0F);
    gShaderProgram.SetVec3fv("uDirLight.ambient", lightAmbient);
    gShaderProgram.SetVec3fv("uDirLight.diffuse", lightDiffuse); // darkened
    gShaderProgram.SetVec3fv("uDirLight.specular", onesVec);
    gShaderProgram.SetVec3fv("uPosLight.ambient", lightAmbient);
    gShaderProgram.SetVec3fv("uPosLight.diffuse", lightDiffuse); // darkened
    gShaderProgram.SetVec3fv("uPosLight.specular", onesVec);
    gShaderProgram.SetVec3fv("uSpotLight.ambient", lightAmbient);
    gShaderProgram.SetVec3fv("uSpotLight.diffuse", lightDiffuse); // darkened
    gShaderProgram.SetVec3fv("uSpotLight.specular", onesVec);
    
    // for positional light
    gShaderProgram.SetVec1f("uPosLight.constant", 1.0F);
    gShaderProgram.SetVec1f("uPosLight.linear", 0.22F);
    gShaderProgram.SetVec1f("uPosLight.quadratic", 0.20F);

    // for spotlight
    gShaderProgram.SetVec1f("uSpotLight.cutoff", glm::cos(glm::radians(12.5F)));
    gShaderProgram.SetVec1f("uSpotLight.outerCutoff", glm::cos(glm::radians(17.5F)));

    // light source shader
    gLightShaderProgram.Use();
    // the light source uses the same vertices/buffer object
    // as the cube above
    gLightShaderProgram.SetMat4fv("uTransform", gLightTransMat);
    gLightShaderProgram.SetVec3fv("uLightColor", lightDiffuse);
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
    //gShaderProgram.Use();
    //updateTransformationMatrix(gModelTransMat, gModelPosition, gCamera);
    //gModelModelMat = glm::mat4(1.0F);
    //gModelModelMat = glm::translate(gModelModelMat, gModelPosition);
    //gShaderProgram.SetMat4fv("uModel", gModelModelMat);
    //gModel.Draw(gShaderProgram);

    // draw the planet
    gShaderProgram.Use();
    gPlanetModelMat = glm::mat4(1.0F);
    gPlanetModelMat = glm::translate(gPlanetModelMat, glm::vec3(0.0F, -3.0F, 0.0F));
    gPlanetModelMat = glm::scale(gPlanetModelMat, glm::vec3(4.0F, 4.0F, 4.0F));
    gShaderProgram.SetMat4fv("uModel", gPlanetModelMat);
    updateTransformationMatrix(gModelTransMat, gPlanetModelMat, gCamera);
    gShaderProgram.SetMat4fv("uTransform", gModelTransMat);
    gPlanetModel.Draw(gShaderProgram);

    // draw the asteroids
    gShaderProgram.Use();
    for (size_t i = 0; i < amount; i++)
    {
        gShaderProgram.SetMat4fv("uModel", gAsteroidModelMats[i]);
        updateTransformationMatrix(gModelTransMat, gAsteroidModelMats[i], gCamera);
        gShaderProgram.SetMat4fv("uTransform", gModelTransMat);
        gAsteroidModel.Draw(gShaderProgram);
    }
    

    // draw the light source
    gLightShaderProgram.Use();
    glBindVertexArray(gLightSource.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
}

int main(void)
{
    initGlfw();
    createWindow();
    initGlad();
    registerGlfwCallbacks();
    srand(glfwGetTime());

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
    gShaderProgram.Create("vertexShader.glsl", "fragmentShader.glsl");
    gShaderProgram.Use();

    // Model.h
    gModel.Load("backpack/backpack.obj");
    gAsteroidModel.Load("rock/rock.obj");
    gPlanetModel.Load("planet/planet.obj");

    // make a shader just for the light source, which uses a different
    // fragment shader and the same vertex shader
    gLightShaderProgram.Create("vertexShader.glsl", "lightFragmentShader.glsl");

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gLightTransMat = createTransformationMatrix();

    // init asteroid positions
    for (size_t i = 0; i < amount; i++)
    {
        glm::mat4 model = glm::mat4(1.0F);
    
        // translation
        float angle = float(i) / float(amount) * 360.0F;
        float displacement = (rand() % int(2 * offset * 100)) / 
                                100.0F - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (rand() % int(2 * offset * 100)) / 
                                100.0F - offset;
        float y = displacement * 0.4F; // keep height smaller
        displacement = (rand() % int(2 * offset * 100)) / 
                                100.0F - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x,y,z));

        // scale
        float scale = (rand() % 20) / 100.0F + 0.05F;
        model = glm::scale(model, glm::vec3(scale));

        // rotation
        float rotAngle = rand() % 360;
        model = glm::rotate(model, rotAngle, glm::vec3(0.4F, 0.6F, 0.8F));

        // add to model mat list
        gAsteroidModelMats[i] = model;
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

    glfwTerminate();
    return 0;
}

