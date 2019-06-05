//
// Test opengl application
//

#include "Window.h"
#include "LoadShaders.h"
#include "Camera.h"
#include "Cube.h"

// matrix library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// STB image loader
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <iostream>

// screen size
const float SCREEN_WIDTH = 640.0f, SCREEN_HEIGHT = 480.0f;

// Texture objects
GLuint texture;
GLuint specular_map;

// cube to draw
GLfloat cube_angle = 0.0f;
Cube cube;

// shader program
GLuint program;

// matrix locations
GLuint modelMatLoc;
GLuint viewMatLoc;
GLuint projMatLoc;

GLuint lampModelMatLoc;
GLuint lampViewMatLoc;
GLuint lampProjMatLoc;

// our transformation matrices
glm::mat4 viewMat;
glm::mat4 projMat;

// camera stuff
Camera camera;

// Light stuff
GLuint vao_light;
GLuint lightVecLoc;
GLuint viewPosLoc;

Cube lamp;
GLuint lampShaderProgram;
glm::vec3 lamp_pos = glm::vec3(1.0f, 0.0f, 0.0f);
GLuint lampLightLoc;
GLuint lampColorLoc;
GLuint lampDiffuseLoc;

//-----------------------------------------------------------------------------------
//
// Init
//
void init_light()
{
    glUseProgram(program);

    // create a light vertex array obj
    glGenVertexArrays(1, &vao_light);
    glBindVertexArray(vao_light);

    // stop using our light VAO
    glBindVertexArray(0);

    // set the light uniform position
    lightVecLoc = glGetUniformLocation(program, "uLightPos");
    glm::vec3 lightPos = lamp.getPos();
    glUniform4fv(lightVecLoc, 1, glm::value_ptr(lightPos));

    // set the light properties
    lampColorLoc = glGetUniformLocation(program, "light.ambient");
    lampDiffuseLoc = glGetUniformLocation(program, "light.diffuse");
    GLuint lampSpecLoc = glGetUniformLocation(program, "light.specular");

    glUniform3f(lampColorLoc, 0.2f, 0.2f, 0.2f);
    glUniform3f(lampDiffuseLoc, 0.5f, 0.5f, 0.5f);
    glUniform3f(lampSpecLoc, 1.0f, 1.0f, 1.0f);

}

void init_texture()
{
    /* 1. Get the texture data from an image */
    int width, height, nChannels;
    unsigned char *data = stbi_load("../container.jpg", &width, &height, &nChannels, 0);
    if (data == NULL) {
        std::cout << "Error loading texture\n";
        exit(EXIT_FAILURE);
    }

    /* 2. Generate an opengl texture */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    /* Set filtering parameters (optional?) */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 3. Generate the texture image from texture data */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* 4. Free data if necessary */
    stbi_image_free(data);

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE0 */
    //glUniform1i(glGetUniformLocation(program, "myTexture"), 0);
    glUniform1i(glGetUniformLocation(program, "material.diffuse"), 0);
}

void init_specular_map()
{
    /* 1. Get the texture data from an image */
    int width, height, nChannels;
    unsigned char *data = stbi_load("../container_specmap.jpg", &width, &height, &nChannels, 0);
    if (data == NULL) {
        std::cout << "Error loading texture\n";
        exit(EXIT_FAILURE);
    }

    /* 2. Generate an opengl texture */
    glGenTextures(1, &specular_map);
    glBindTexture(GL_TEXTURE_2D, specular_map);

    /* Set filtering parameters (optional?) */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 3. Generate the texture image from texture data */
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* 4. Free data if necessary */
    stbi_image_free(data);

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE1 */
    GLuint tex2loc = glGetUniformLocation(program, "material.specular");
    glUniform1i(tex2loc, 1);
}

void init_matuniform()
{
// ------------------------------------------------------------------------------------
//
// Projection Matrix
//
    /* Create a persepective matrix */
    projMat = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH/SCREEN_HEIGHT, 0.1f, 100.0f);

    /* Send the matrices to the shader
     * The GL_FALSE arg is important here - if true, OpenGL will transpose/swap rows and cols
     * We use column-major ordering so no need to switch (as opposed to row major like in mathematics)
     */
    modelMatLoc = glGetUniformLocation(program, "uModelMat");
    viewMatLoc = glGetUniformLocation(program, "uViewMat");
    projMatLoc = glGetUniformLocation(program, "uProjMat");

    //glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    // send the model matrix location to the cube
    cube.setModelMatLoc(modelMatLoc);

// -----------------------------------------------------------------------------------
//
// Lamp Shader Uniforms
//
    glUseProgram(lampShaderProgram);
    lampModelMatLoc = glGetUniformLocation(lampShaderProgram, "uModelMat");
    lampViewMatLoc = glGetUniformLocation(lampShaderProgram, "uViewMat");
    lampProjMatLoc = glGetUniformLocation(lampShaderProgram, "uProjMat");

    glUniformMatrix4fv(lampProjMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    lamp.setModelMatLoc(lampModelMatLoc);

    glUseProgram(program);
    lampLightLoc = glGetUniformLocation(program, "uLightPos");
    glm::vec3 lampPos = lamp.getPos();
    glUniform3f(lampLightLoc, lampPos.x, lampPos.y, lampPos.z);

}

void init(void)
{
    // regular object shader
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert", 0 },
        { GL_FRAGMENT_SHADER, "triangles.frag", 0},
        { GL_NONE, NULL, 0 }
    };

    program = LoadShaders(shaders);
    if (program == 0) {
        std::cout << "Error loading shaders!\n";
        exit(EXIT_FAILURE);
    }
    glUseProgram(program);

    // lamp shader (so color doesn't affect the lamp)
    ShaderInfo lampShaders[] = {
        { GL_VERTEX_SHADER, "lampShader.vert", 0 },
        { GL_FRAGMENT_SHADER, "lampShader.frag", 0},
        { GL_NONE, NULL, 0 }
    };
    lampShaderProgram = LoadShaders(lampShaders);
    if (lampShaderProgram == 0) {
        std::cout << "Error loading lamp shader!\n";
        exit(EXIT_FAILURE);
    }

    // tell the cube to use this shader
    cube.setShaderProg(program);
    cube.init();
    cube.move(glm::vec3(-1.0f, -1.0f, -1.0f));

    // set cube material properties
    GLuint cubeAmbLoc = glGetUniformLocation(program, "material.ambient");
    GLuint cubeDiffLoc = glGetUniformLocation(program, "material.diffuse");
    GLuint cubeSpecLoc = glGetUniformLocation(program, "material.specular");
    GLuint cubeShiniLoc = glGetUniformLocation(program, "material.shininess");

    glUniform3f(cubeAmbLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(cubeDiffLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(cubeSpecLoc, 0.5f, 0.5f, 0.5f);
    glUniform1f(cubeShiniLoc, 32.0f);

    lamp.setShaderProg(lampShaderProgram);
    lamp.init();
    lamp.move(glm::vec3(0.5f, 0.0f, 0.0f));

    init_texture();
    init_specular_map();
    init_matuniform();
    init_light();

    // we're doing 3d now so we want opengl to check who's in front of who
    glEnable(GL_DEPTH_TEST);
}

void update_matrices()
{
    glUseProgram(program);
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, camera.getLookAtPtr());
    glUniformMatrix4fv(projMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));

    glUniform3fv(viewPosLoc, 1, camera.getPosPtr());

    glUseProgram(lampShaderProgram);
    glUniformMatrix4fv(lampViewMatLoc, 1, GL_FALSE, camera.getLookAtPtr());
    glUniformMatrix4fv(lampProjMatLoc, 1, GL_FALSE, glm::value_ptr(projMat));
}


//---------------------------------------------------------------------------------
//
// Display
//
void display(void)
{
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    /* Clear screen and z buffer */
    glClearBufferfv(GL_COLOR, 0, black);
    glClear(GL_DEPTH_BUFFER_BIT); // will have a blank screen without this

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, specular_map);

    cube.draw();

    lamp.draw();
}


//---------------------------------------------------------------------------------
//
// Main
//
int main(int argc, char *argv[])
{
    Window window;
    if (!window.init(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello world")) {
        return -1;
    }

    camera.setWindow(&window);

    // create buffers
    init();


    while(!window.shouldClose())
    {
        // rotate the cube
        static GLfloat angle = 0.0f;
        angle += 1.0f;
        cube.rotate(angle, glm::vec3(1.0f, 0.5f, 0.0f));

        camera.update();
        update_matrices();
        display();
        window.update();
    }

    return 0;
}

