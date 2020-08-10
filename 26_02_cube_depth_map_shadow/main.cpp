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
Texture gWoodTexture;

LightSource gLightSource;
glm::vec3 gLightPosition = glm::vec3(-2.0f, 1.0f, -1.0F);
glm::vec3 gLightDirection = glm::vec3(-0.2F, -1.0F, -0.3F);
Cube gCube;
glm::vec3 gCubePosition = glm::vec3(0.0F, 0.0F, 0.0F);
glm::mat4 gCubeModelMat;
std::vector<glm::vec3> gCubePositions = {
    glm::vec3(-1.0f, -2.0f, -1.0f),
    glm::vec3(2.0f, -3.5f, 0.0f)
};

RoomCube gRoomCube;
glm::mat4 gRoomModelMat;

Floor gFloor;
glm::vec3 gFloorPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::mat4 gFloorModelMat;
glm::mat4 gFloorTransMat;

Shader gVertexShader;
Shader gFragmentShader;
ShaderProgram gShaderProgram;

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

// for depth-mapped shadows
Shader gDepthVertexShader;
Shader gDepthGeometryShader;
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

// called once every frame during main loop
static void draw()
{
    // shader uniform locations
    glUseProgram(gDepthShaderProgram.id);
    static GLuint uModel_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uModel");
    static GLuint uLightTransform0_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightTransforms[0]");
    static GLuint uLightTransform1_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightTransforms[1]");
    static GLuint uLightTransform2_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightTransforms[2]");
    static GLuint uLightTransform3_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightTransforms[3]");
    static GLuint uLightTransform4_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightTransforms[4]");
    static GLuint uLightTransform5_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightTransforms[5]");
    static GLuint uLightPos_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uLightPos");
    static GLuint uFarPlane_lightSpace = glGetUniformLocation(gDepthShaderProgram.id, "uFarPlane");

    glUseProgram(gShaderProgram.id);
    static GLuint uProjection = glGetUniformLocation(gShaderProgram.id, "uProjection");
    static GLuint uView = glGetUniformLocation(gShaderProgram.id, "uView");
    static GLuint uModel = glGetUniformLocation(gShaderProgram.id, "uModel");
    static GLuint uDiffuseTex = glGetUniformLocation(gShaderProgram.id, "uDiffuseTex");
    static GLuint uShadowMapTex = glGetUniformLocation(gShaderProgram.id, "uShadowMapTex");
    static GLuint uLightPos = glGetUniformLocation(gShaderProgram.id, "uLightPos");
    static GLuint uViewPos = glGetUniformLocation(gShaderProgram.id, "uViewPos");
    static GLuint uFarPlane = glGetUniformLocation(gShaderProgram.id, "uFarPlane");

    glUseProgram(gLightShaderProgram.id);
    static GLuint uTransform_light = glGetUniformLocation(gLightShaderProgram.id, "uTransform");
    static GLuint uLightColor_light = glGetUniformLocation(gLightShaderProgram.id, "uLightColor");

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
    //glActiveTexture(GL_TEXTURE0);

    // TODO - does it matter if this goes here, or does it go later?
    //glActiveTexture(GL_TEXTURE1);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, gDepthMap.textureID);
    //glUniform1i()
    glCullFace(GL_FRONT); 

        // send light properties to depth shader
        glUniform3fv(uLightPos_lightSpace, 1, glm::value_ptr(gLightPosition));
        glUniform1f(uFarPlane_lightSpace, 25.0f); // far plane

        // calculate light matrices
        // (this is a directional light)
        //float nearPlane = 1.0f;
        //float farPlane = 7.5f;
        //glm::mat4 lightProjMat = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f,
        //                                    nearPlane, farPlane);
        
        // 90 degrees is important here so we viewing field is exactly large enough
        // to fill a single face of the cubemap such that all faces align correctly
        // at the edges
        glm::mat4 lightProjMat = glm::perspective(glm::radians(90.0f),
                                    float(gDepthMap.width)/float(gDepthMap.height),
                                    1.0f,
                                    25.0f);
        // light position. this time we have a positional light,
        // then we look at each wall of the cube map
        static glm::mat4 lightSpaceMats[6];// = lightProjMat * lightViewMat;
#define GEN_LIGHT_MAT(dir, up) lightProjMat * glm::lookAt(gLightPosition, gLightPosition + dir, up)
        lightSpaceMats[0] = GEN_LIGHT_MAT(glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        lightSpaceMats[1] = GEN_LIGHT_MAT(glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
        lightSpaceMats[2] = GEN_LIGHT_MAT(glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));
        lightSpaceMats[3] = GEN_LIGHT_MAT(glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0));
        lightSpaceMats[4] = GEN_LIGHT_MAT(glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));
        lightSpaceMats[5] = GEN_LIGHT_MAT(glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0));
#undef GEN_LIGHT_MAT

        //static GLuint depthTransformLoc = 
        //    glGetUniformLocation(gDepthShaderProgram.id, "uTransform");
        //glUniformMatrix4fv(depthTransformLoc,
        //                    1,
        //                    GL_FALSE,
        //                    glm::value_ptr(lightSpaceMat));
#define UPDATE_LIGHT_MAT(loc, mat) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat))
    UPDATE_LIGHT_MAT(uLightTransform0_lightSpace, lightSpaceMats[0]);
    UPDATE_LIGHT_MAT(uLightTransform1_lightSpace, lightSpaceMats[1]);
    UPDATE_LIGHT_MAT(uLightTransform2_lightSpace, lightSpaceMats[2]);
    UPDATE_LIGHT_MAT(uLightTransform3_lightSpace, lightSpaceMats[3]);
    UPDATE_LIGHT_MAT(uLightTransform4_lightSpace, lightSpaceMats[4]);
    UPDATE_LIGHT_MAT(uLightTransform5_lightSpace, lightSpaceMats[5]);
#undef UPDATE_LIGHT_MAT
    
#if 0
        // draw the floor
        gFloorModelMat = glm::mat4(1.0F);
        gFloorModelMat = glm::translate(gFloorModelMat, gFloorPosition);
        //static GLuint depthShaderModelLoc = 
        //    glGetUniformLocation(gDepthShaderProgram.id, "uModel");
        //glUniformMatrix4fv(depthShaderModelLoc,
        //    1,
        //    GL_FALSE,
        //    glm::value_ptr(gFloorModelMat));
        glUniformMatrix4fv(uModel_lightSpace, 1, GL_FALSE, glm::value_ptr(gFloorModelMat));
        glBindVertexArray(gFloor.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gFloor.numVertices);
#endif

        // draw the room cube
        //static GLuint depthShaderModelLoc = 
        //    glGetUniformLocation(gDepthShaderProgram.id, "uModel");
        //glUniformMatrix4fv(depthShaderModelLoc,
        //    1,
        //    GL_FALSE,
        //    glm::value_ptr(gFloorModelMat));
        glUniformMatrix4fv(uModel_lightSpace, 1, GL_FALSE, glm::value_ptr(gRoomModelMat));
        glBindVertexArray(gRoomCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gRoomCube.numVertices);


        // draw some cubes
        for (const glm::vec3& position : gCubePositions)
        {
            gCubeModelMat = glm::mat4(1.0F);
            gCubeModelMat = glm::translate(gCubeModelMat, position);
            //glUniformMatrix4fv(depthShaderModelLoc,
            //    1,
            //    GL_FALSE,
            //    glm::value_ptr(gCubeModelMat));
            glUniformMatrix4fv(uModel_lightSpace, 1, GL_FALSE, glm::value_ptr(gCubeModelMat));
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

    // set positional properties
    glUniform3fv(uLightPos, 1, glm::value_ptr(gLightPosition));
    glUniform3fv(uViewPos, 1, glm::value_ptr(gCamera.position));
    glUniform1f(uFarPlane, 25.0f); // far plane
    
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(uDiffuseTex, 0); // texture 0
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gDepthMap.textureID);
    glUniform1i(uShadowMapTex, 1); // texture 1

    // update the view and projection matrices
    glm::mat4 gProjMat = glm::perspective(glm::radians(gCamera.FOV),
        float(WINDOW_WIDTH) / float(WINDOW_HEIGHT),
        0.1F,
        100.0F);
    glm::mat4 gViewMat = glm::lookAt(gCamera.position,
        gCamera.position + gCamera.front,
        gCamera.up);
    //glUniformMatrix4fv(gUniformLocations["uProj"],
    //    1, // number of matrices
    //    GL_FALSE, // should the matrices be transposed?
    //    glm::value_ptr(gProjMat)); // pointer to data
    glUniformMatrix4fv(uProjection, 1, GL_FALSE, glm::value_ptr(gProjMat));
    //glUniformMatrix4fv(gUniformLocations["uView"],
    //    1, // number of matrices
    //    GL_FALSE, // should the matrices be transposed?
    //    glm::value_ptr(gViewMat)); // pointer to data
    glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(gViewMat));
    // calculated above
    //glUniformMatrix4fv(gUniformLocations["uLightSpaceMat"],
    //    1, // number of matrices
    //    GL_FALSE, // should the matrices be transposed?
    //    glm::value_ptr(lightSpaceMat)); // pointer to data

#if 0
    // draw the floor
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWoodTexture.id);
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
    // draw the room
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWoodTexture.id);
    glUniform1i(uDiffuseTex, 0); // texture 0
    // Bind our previously used depth map texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, gDepthMap.textureID);
    glUniform1i(uShadowMapTex, 1); // texture 1
    glUniformMatrix4fv(uModel, 1, GL_FALSE, glm::value_ptr(gRoomModelMat));
    glBindVertexArray(gRoomCube.VAO);
    glDrawArrays(GL_TRIANGLES, 0, gRoomCube.numVertices);
    

    // draw some cubes
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gWoodTexture.id);
    glUniform1i(uDiffuseTex, 0); // texture 0
    for (const glm::vec3& position : gCubePositions)
    {
        gCubeModelMat = glm::mat4(1.0F);
        gCubeModelMat = glm::translate(gCubeModelMat, position);
        glUniformMatrix4fv(uModel,
            1,
            GL_FALSE,
            glm::value_ptr(gCubeModelMat));
        glBindVertexArray(gCube.VAO);
        glDrawArrays(GL_TRIANGLES, 0, gCube.numVertices);
    }

    // draw the light source
    glUseProgram(gLightShaderProgram.id);
    glUniformMatrix4fv(uTransform_light, 1, GL_FALSE, glm::value_ptr(gLightTransMat));
    glUniform3f(uLightColor_light, 1.0f, 1.0f, 1.0f); // light color is white
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

    // RoomCube.h
    gRoomCube = createRoomCube();
    gRoomModelMat = glm::mat4(1.0f); // located at origin

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

    // Frame buffer drawn to screen shader
    gScreenVertexShader = createVertexShader("vertexShader_screenTexture.glsl");
    gScreenFragmentShader = createFragmentShader("fragmentShader_screenTexture.glsl");
    gScreenShaderProgram = createShaderProgram(gScreenVertexShader, gScreenFragmentShader);

    // depth shader in light space for depth shadows
    std::cout << "Creating depth shader" << std::endl;
    gDepthVertexShader = createVertexShader("vertexShader_lightSpace.glsl");
    gDepthFragmentShader = createFragmentShader("fragmentShader_lightSpace.glsl");
    gDepthGeometryShader = createGeometryShader("geometryShader_lightSpace.glsl");
    gDepthShaderProgram = createShaderProgram(gDepthVertexShader, gDepthGeometryShader,
        gDepthFragmentShader);

    // make a shader just for the light source
    std::cout << "Creating light shader" << std::endl;
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
        static float lightMoveRadius = 5.0F;
        gLightPosition.x = cos(glfwGetTime()) * lightMoveRadius;
        gLightDirection.x = cos(glfwGetTime()) * lightMoveRadius;

        // Transform.h
        updateTransformationMatrix(gCubeTransMat, gCubePosition, gCamera);
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

