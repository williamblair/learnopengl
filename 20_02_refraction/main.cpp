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
#include "SkyBox.h"
#include "SkyBoxCube.h"
#include "Model.h"

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

Floor gFloor;
glm::vec3 gFloorPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 gFloorModelMat;
glm::mat4 gFloorTransMat;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;
std::map<std::string, int> gUniformLocations;

// shaders just used by the object representing the light
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram;

glm::mat4 gCubeTransMat;
glm::mat4 gLightTransMat;
Camera gCamera;


Shader gSkyBoxVertexShader;
Shader gSkyBoxFragmentShader;
ShaderProgram gSkyBoxShaderProgram;
SkyBox gSkyBox;
// order matters here
std::vector<std::string> gSkyBoxFaces = {
    "skybox/right.jpg",
    "skybox/left.jpg",
    "skybox/top.jpg",
    "skybox/bottom.jpg",
    "skybox/front.jpg",
    "skybox/back.jpg"
};
SkyBoxCube gSkyBoxCube;

Model gModel;
glm::mat4 gModelTransMat;
glm::mat4 gModelModelMat;
glm::vec3 gModelPosition = glm::vec3(0.0f, 0.0f, 0.0f);
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
        glm::value_ptr(gCubeTransMat)); // pointer to data
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
        glm::value_ptr(gCubeModelMat));

    // main shader: cube material properties
    glUniform1i(gUniformLocations["uMaterial.diffuse"], 0); // texture 0
    //glUniform3f(gUniformLocations["uMaterial.specular"], 0.5f, 0.5F, 0.5F);
    glUniform1i(gUniformLocations["uMaterial.specular"], 1); // texture 1
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

    // for Sky Box reflection
    glUniform1i(gUniformLocations["uSkyBoxSampler"], 0); // first texture

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

    // for the skybox, so that way its drawn when its depth is equal to 1.0
    // (1.0 == 1.0 which is max depth)
    glDepthFunc(GL_LEQUAL);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyBox.id); // for reflection

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
    glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyBox.id); // for reflection
    gModel.Draw(gShaderProgram);

    // draw the cube
    glUseProgram(gShaderProgram.id);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gDiffuseMap.id);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gSpecularMap.id);
    glPolygonMode(GL_FRONT_AND_BACK, gCube.renderMode);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyBox.id); // for reflection
    for (glm::vec3& position : gCubePositions)
    {
        updateTransformationMatrix(gCubeTransMat, position, gCamera);
        glUniformMatrix4fv(gUniformLocations["uTransform"],
            1, // number of matrices
            GL_FALSE, // should the matrices be transposed?
            glm::value_ptr(gCubeTransMat)); // pointer to data
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

    // draw the background skybox (last so other areas will already be covered for some optimization)
    //glDepthMask(GL_FALSE); // false so always drawn in background
    //glDepthFunc(GL_LESS); // only draw if depth is LESS THAN the existing pixel
    glUseProgram(gSkyBoxShaderProgram.id);
    // set view and projection matrix
    static GLuint skyBoxProjLoc = glGetUniformLocation(gSkyBoxShaderProgram.id, "uProjection");
    static GLuint skyBoxViewLoc = glGetUniformLocation(gSkyBoxShaderProgram.id, "uView");
    glm::mat4 viewMat = glm::lookAt(
        gCamera.position,
        gCamera.position + gCamera.front,
        gCamera.up);
    // remove the translation component from the view matrix, so the skybox doesn't move
    viewMat = glm::mat4(glm::mat3(viewMat));
    static float aspectRatio = 800.0F / 600.0F;
    static float nearClip = 0.1F;
    static float farClip = 100.0F;
    glm::mat4 projMat = glm::perspective(glm::radians(gCamera.FOV),
        aspectRatio,
        nearClip,
        farClip);
    glUniformMatrix4fv(skyBoxViewLoc,
        1,
        GL_FALSE,
        glm::value_ptr(viewMat));
    glUniformMatrix4fv(skyBoxProjLoc,
        1,
        GL_FALSE,
        glm::value_ptr(projMat));
    glBindVertexArray(gSkyBoxCube.VAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gSkyBox.id);
    glDrawArrays(GL_TRIANGLES, 0, gSkyBoxCube.numVertices);
    //glDepthMask(GL_TRUE);
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

    gUniformLocations["uCameraPosition"] = glGetUniformLocation(gShaderProgram.id, "uCameraPosition");
    gUniformLocations["uTransform"] = glGetUniformLocation(gShaderProgram.id, "uTransform");
    gUniformLocations["uModel"] = glGetUniformLocation(gShaderProgram.id, "uModel");
    gUniformLocations["uMaterial.diffuse"] = glGetUniformLocation(gShaderProgram.id, "uMaterial.diffuse");
    gUniformLocations["uMaterial.specular"] = glGetUniformLocation(gShaderProgram.id, "uMaterial.specular");
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
    // for skybox texture sampler
    gUniformLocations["uSkyBoxSampler"] = glGetUniformLocation(gShaderProgram.id, "uSkyBoxSampler");

    // make a shader just for the light source, which uses a different
    // fragment shader and the same vertex shader
    gLightFragmentShader = createFragmentShader("lightFragmentShader.glsl");
    gLightShaderProgram = createShaderProgram(gVertexShader,
        gLightFragmentShader);

    // Shader for the cube map/skybox
    gSkyBoxVertexShader = createVertexShader("vertexShader_skyBox.glsl");
    gSkyBoxFragmentShader = createFragmentShader("fragmentShader_skyBox.glsl");
    gSkyBoxShaderProgram = createShaderProgram(gSkyBoxVertexShader, gSkyBoxFragmentShader);

    // LightSource.h
    gLightSource = createLightSource(gCube);

    // Transform.h
    gCubeTransMat = createTransformationMatrix();
    gLightTransMat = createTransformationMatrix();

    // Texture.h
    gDiffuseMap = createTexture("marble.jpg");
    gSpecularMap = createTexture("container2_specular.png");
    gMetalTexture = createTexture("metal.png");

    // SkyBox.h
    gSkyBox = createSkyBox(gSkyBoxFaces);

    // SkyBoxCube.h
    gSkyBoxCube = createSkyBoxCube();

    // Model.h
    gModel.Load("backpack/backpack.obj");

    while (!glfwWindowShouldClose(gWindow))
    {
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gWindow, true);
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

    glDeleteShader(gVertexShader.id);
    glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

