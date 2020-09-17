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
#include "Sphere.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Transform.h"
#include "Camera.h"
#include "LightSource.h"
#include "Floor.h"
#include "IrradianceCubemap.h"
#include "IrradiancePrecomputedMap.h"

// Globals
const size_t WINDOW_WIDTH = 800;
const size_t WINDOW_HEIGHT = 600;
GLFWwindow* gWindow = nullptr;

Texture gDiffuseMap;
Texture gSpecularMap;
Texture gWoodTexture;

Texture gHDRRadianceTex;

std::vector<glm::vec3> gLightPositions = {
    glm::vec3(-10.0f,  10.0f, 10.0f),
    glm::vec3( 10.0f,  10.0f, 10.0f),
    glm::vec3(-10.0f, -10.0f, 10.0f),
    glm::vec3( 10.0f, -10.0f, 10.0f)
};
std::vector<glm::vec3> gLightColors = {
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f),
    glm::vec3(300.0f, 300.0f, 300.0f)
};

Sphere gSphere;
int numSphereRows = 7;
int numSphereCols = 7;
float sphereSpacing = 2.5f;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;
//std::map<std::string, int> gUniformLocations;

// shaders just used by the object representing the light
Shader gLightFragmentShader;
ShaderProgram gLightShaderProgram;

// Shader to convert from the equirectangular HDR irradiance map to a cube map
Shader gEqui2CubeVertexShader;
Shader gEqui2CubeFragmentShader;
ShaderProgram gEqui2CubeShaderProgram;

Shader gDebugIrradianceCubeVertexShader;
Shader gDebugIrradianceCubeFragmentShader;
ShaderProgram gDebugIrradianceCubeShaderProgram;
Cube gDebugEquiCube;

Shader gPrecomputeIrradianceVertexShader;
Shader gPrecomputeIrradianceFragmentShader;
ShaderProgram gPrecomputeIrradianceShaderProgram;

IrradianceCubemap gIrradianceCubemap;
std::vector<glm::mat4> gIrradianceViews = {
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f, 0.0f),
                glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, 1.0f),
                glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, 0.0f, -1.0f),
                glm::vec3(0.0f, -1.0f, 0.0f))
};
glm::mat4 gIrradianceProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);

IrradiancePrecomputedMap gIrradiancePrecomputedMap;
bool gUseIrradiance = true;

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
    static GLuint uAlbedo = glGetUniformLocation(gShaderProgram.id, "uAlbedo");
    static GLuint uMetallic = glGetUniformLocation(gShaderProgram.id, "uMetallic");
    static GLuint uRoughness = glGetUniformLocation(gShaderProgram.id, "uRoughness");
    static GLuint uAo = glGetUniformLocation(gShaderProgram.id, "uAo");
    static GLuint uTransform = glGetUniformLocation(gShaderProgram.id, "uTransform");
    static GLuint uModel = glGetUniformLocation(gShaderProgram.id, "uModel");
    static GLuint uIrradianceMap = glGetUniformLocation(gShaderProgram.id, "uIrradianceMap");

    // clear the screen
    GLfloat r = 0.2F; // red
    GLfloat g = 0.0F; // green
    GLfloat b = 0.1F; // blue
    GLfloat a = 1.0F; // alpha
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gShaderProgram.id);

    // send lights to the shader
    for (size_t i = 0; i < gLightPositions.size(); i++)
    {
        std::string str = "uLightPositions[" + std::to_string(i) + "]";
        GLuint uLightPos = glGetUniformLocation(gShaderProgram.id, str.c_str());
        glUniform3fv(uLightPos, 1, glm::value_ptr(gLightPositions[i]));
    }
    for (size_t i = 0; i < gLightColors.size(); i++)
    {
        std::string str = "uLightColors[" + std::to_string(i) + "]";
        GLuint uLightColor = glGetUniformLocation(gShaderProgram.id, str.c_str());
        glUniform3fv(uLightColor, 1, glm::value_ptr(gLightColors[i]));
    }

    // send material properties to the shader
    glUniform3f(uAlbedo, 0.5f, 0.0f, 0.0f);
    glUniform1f(uAo, 1.0f);

    // send camera position to the shader
    static GLuint uCamPos = glGetUniformLocation(gShaderProgram.id, "uCamPos");
    glUniform3fv(uCamPos, 1, glm::value_ptr(gCamera.position));

    // Set the irradiance map in the shader
    glUniform1i(uIrradianceMap, 0); // GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gIrradiancePrecomputedMap.textureID);

    // toggle whether to use irradiance
    static GLuint uUseIrradiance = glGetUniformLocation(gShaderProgram.id, "uUseIrradiance");
    glUniform1i(uUseIrradiance, gUseIrradiance);

    // draw the spheres
    glm::mat4 transMat;
    glm::mat4 modelMat;
    for (size_t row = 0; row < numSphereRows; ++row)
    {
        glUniform1f(uMetallic, float(row) / float(numSphereRows));

        for (size_t col = 0; col < numSphereCols; ++col)
        {
            // clamp roughness to [0.05,1.0] because roughness of 0.0 looks "off" in direct lighting"
            glUniform1f(uRoughness, glm::clamp(float(col) / float(numSphereCols), 0.025f, 1.0f));

            glm::vec3 pos((float(col) - float((numSphereCols / 2))) * sphereSpacing,
                          (float(row) - float((numSphereRows / 2))) * sphereSpacing,
                          0.0f);
            updateTransformationMatrix(transMat, pos, gCamera);
            modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, pos);

            glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(modelMat));
            glUniformMatrix4fv(uTransform, 1, GL_FALSE, glm::value_ptr(transMat));

            glBindVertexArray(gSphere.VAO);
            glDrawElements(GL_TRIANGLE_STRIP, gSphere.numIndices, GL_UNSIGNED_INT, 0);
        }
    }


    // Draw the cubemap environment
    glDepthFunc(GL_LEQUAL);
    glUseProgram(gDebugIrradianceCubeShaderProgram.id);
    static GLint uView_debugEqui = glGetUniformLocation(gDebugIrradianceCubeShaderProgram.id, "uView");
    static GLint uProjection_debugEqui = glGetUniformLocation(gDebugIrradianceCubeShaderProgram.id, "uProjection");
    static GLint uEnvMap =  glGetUniformLocation(gDebugIrradianceCubeShaderProgram.id, "uEnvMap");
    glm::mat4 projMat = glm::perspective(glm::radians(45.0f),
        float(WINDOW_WIDTH)/float(WINDOW_HEIGHT),
        0.1f,
        100.0f);
    glUniformMatrix4fv(uProjection_debugEqui, 1, GL_FALSE, glm::value_ptr(projMat));
    glm::mat4 viewMat = glm::lookAt(gCamera.position, gCamera.position + gCamera.front, gCamera.up);
    glUniformMatrix4fv(uView_debugEqui, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniform1i(uEnvMap, 0); // GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, gIrradianceCubemap.textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gIrradiancePrecomputedMap.textureID);

        glBindVertexArray(gDebugEquiCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gDebugEquiCube.numVertices);

    // restore the default depth func
    glDepthFunc(GL_LESS);
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

    // Sphere.h
    std::cout << "Creating sphere" << std::endl;
    gSphere = createSphere();

    // Camera.h
    gCamera = createCamera();

    // Cube.h
    gDebugEquiCube = createCube();

    // IrradianceCubemap.h
    gIrradianceCubemap = createIrradianceCubemap();

    // IrradiancePrecomputedMap.h
    gIrradiancePrecomputedMap = createIrradiancePrecomputedMap();

    // Shader.h/ShaderProgram.h
    std::cout << "Creating main shader program" << std::endl;
    gVertexShader = createVertexShader("vertexShader.glsl");
    gFragmentShader = createFragmentShader("fragmentShader.glsl");
    gShaderProgram = createShaderProgram(gVertexShader,
        gFragmentShader);
    glUseProgram(gShaderProgram.id);

    // make a shader just for the light source, which uses a different
    // fragment shader and the same vertex shader
    gLightFragmentShader = createFragmentShader("lightFragmentShader.glsl");
    gLightShaderProgram = createShaderProgram(gVertexShader,
        gLightFragmentShader);

    // create the equirectangular to cubemap shader
    std::cout << "Creating equirectangular to cubemap shader" << std::endl;
    gEqui2CubeVertexShader = createVertexShader("vertexShader_equirectangularToCubemap.glsl");
    gEqui2CubeFragmentShader = createFragmentShader("fragmentShader_equirectangularToCubemap.glsl");
    gEqui2CubeShaderProgram = createShaderProgram(gEqui2CubeVertexShader, 
        gEqui2CubeFragmentShader);

    // create the irradiance cube map debug shader
    std::cout << "Creating irradiance cube map debug shader" << std::endl;
    gDebugIrradianceCubeVertexShader = createVertexShader("vertexShader_debugIrradianceCubemap.glsl");
    gDebugIrradianceCubeFragmentShader = createFragmentShader("fragmentShader_debugIrradianceCubemap.glsl");
    gDebugIrradianceCubeShaderProgram = createShaderProgram(gDebugIrradianceCubeVertexShader, gDebugIrradianceCubeFragmentShader);

    // create the precompute irradiance shader
    std::cout << "Creating precompute irradiance shader" << std::endl;
    gPrecomputeIrradianceVertexShader = createVertexShader("vertexShader_equirectangularToCubemap.glsl");
    gPrecomputeIrradianceFragmentShader = createFragmentShader("fragmentShader_preComputeIrradiance.glsl");
    gPrecomputeIrradianceShaderProgram = createShaderProgram(gPrecomputeIrradianceVertexShader, 
        gPrecomputeIrradianceFragmentShader);

    // Texture.h
    gDiffuseMap = createTexture("marble.jpg");
    gSpecularMap = createTexture("container2_specular.png");
    gWoodTexture = createTexture("wood.png");
    gHDRRadianceTex = createHDRTexture("newport_loft.hdr");

    // Convert the HDR irradiant texture to a cubemap texture
    glBindFramebuffer(GL_FRAMEBUFFER, gIrradianceCubemap.FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gIrradianceCubemap.RBO);
    glUseProgram(gEqui2CubeShaderProgram.id);
    static GLint uView_equiConvert = glGetUniformLocation(gEqui2CubeShaderProgram.id, "uView");
    static GLint uProjection_equiConvert = glGetUniformLocation(gEqui2CubeShaderProgram.id, "uProjection");
    static GLint uEquirectangularMap =  glGetUniformLocation(gEqui2CubeShaderProgram.id, "uEquirectangularMap");
    glUniformMatrix4fv(uProjection_equiConvert, 1, GL_FALSE, glm::value_ptr(gIrradianceProjection));
    glUniform1i(uEquirectangularMap, 0); // GL_TEXTURE0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gHDRRadianceTex.id);

    glViewport(0, 0, 512, 512); // texture size is 512
    glBindFramebuffer(GL_FRAMEBUFFER, gIrradianceCubemap.FBO);
    for (size_t i = 0; i < gIrradianceViews.size(); i++)
    {
        glUniformMatrix4fv(uView_equiConvert, 1, GL_FALSE, glm::value_ptr(gIrradianceViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, 
                               GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               gIrradianceCubemap.textureID,
                               0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray(gDebugEquiCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gDebugEquiCube.numVertices);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Precompute Irradiance using the Irradiance cubmap texture, convoluting it
    glBindFramebuffer(GL_FRAMEBUFFER, gIrradianceCubemap.FBO);
    glBindRenderbuffer(GL_RENDERBUFFER, gIrradianceCubemap.RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32); // match the precompute irradiance texture size
    glUseProgram(gPrecomputeIrradianceShaderProgram.id);
    glUniform1i(glGetUniformLocation(gPrecomputeIrradianceShaderProgram.id, "uEnvMap"), 0); // GL_TEXTURE0
    static GLuint uProjection_precompute = glGetUniformLocation(gPrecomputeIrradianceShaderProgram.id, "uProjection");
    static GLuint uView_precompute = glGetUniformLocation(gPrecomputeIrradianceShaderProgram.id, "uView");
    glUniformMatrix4fv(uProjection_precompute, 1, GL_FALSE, glm::value_ptr(gIrradianceProjection));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gIrradianceCubemap.textureID);
    glViewport(0, 0, 32, 32); // match precompute irradiance texture size
    glBindFramebuffer(GL_FRAMEBUFFER, gIrradianceCubemap.FBO); 
    for (size_t i = 0; i < 6; i++)
    {
        glUniformMatrix4fv(uView_precompute, 1, GL_FALSE, glm::value_ptr(gIrradianceViews[i]));
        glFramebufferTexture2D(GL_FRAMEBUFFER, 
                               GL_COLOR_ATTACHMENT0, 
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               gIrradiancePrecomputedMap.textureID, 
                               0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(gDebugEquiCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gDebugEquiCube.numVertices);
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // restore the viewport
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    while (!glfwWindowShouldClose(gWindow))
    {
        if (glfwGetKey(gWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(gWindow, true);
        }
        // press space to switch between using irradiance or ambiance of 0.03
        else if (glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
            gUseIrradiance = !gUseIrradiance;
            std::cout << "Use Irradiance: " << gUseIrradiance << std::endl;
        }

        moveCamera();

        // move the light around
        //static float lightMoveRadius = 5.0F;
        //gLightPosition.x = cos(glfwGetTime()) * lightMoveRadius;
        //gLightDirection.x = cos(glfwGetTime()) * lightMoveRadius;

        draw();

        glfwSwapBuffers(gWindow);
        glfwPollEvents();
    }

    glDeleteShader(gVertexShader.id);
    glDeleteShader(gFragmentShader.id);
    glfwTerminate();
    return 0;
}

