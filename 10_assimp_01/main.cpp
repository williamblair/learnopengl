//
// Test opengl application
//

#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Cube.h"
#include "Mesh.h"
#include "Model.h"
#include "Texture.h"
#include "Light.h"
#include "Material.h"

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
Texture texture, texture2;

// cube to draw
GLfloat cube_angle = 0.0f;
Cube cube;

// shader program
GLuint program;
Shader mainShader;

// matrix locations
// TODO - better system for this; just internalize or something
GLuint modelMatLoc;

// our transformation matrices
glm::mat4 viewMat;
glm::mat4 projMat;

// camera stuff
Camera camera;

// Light stuff
GLuint lampShaderProgram;
Light light;

//-----------------------------------------------------------------------------------
//
// Init
//
void init_light()
{
    light.SetWorldShaderProgram( program );
}

void init_texture()
{
    if ( !texture.Load("container.jpg", "../") ) {
        exit( EXIT_FAILURE );
    }

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE0 */
    glUniform1i( glGetUniformLocation(program, "myTexture"), 0 );
}

void init_texture2()
{
    if ( !texture2.Load("awesomeface.png", "../") ) {
        exit(EXIT_FAILURE);
    }

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE1 */
    GLuint tex2loc = glGetUniformLocation(program, "myTexture2");
    glUniform1i(tex2loc, 1);
}

void init_matuniform()
{
//    GLuint program = mainShader.GetProgram();

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

    mainShader.SetProjMat( glm::value_ptr(projMat) );

    // send the model matrix location to the cube
    cube.setModelMatLoc( modelMatLoc );

// -----------------------------------------------------------------------------------
//
// Lamp Shader Uniforms
//
    light.GetShader().SetProjMat( glm::value_ptr(projMat) );
}

void init(void)
{
    if ( !mainShader.Load("triangles.vert", "triangles.frag") ) {
        exit(EXIT_FAILURE);
    }
    program = mainShader.GetProgram();
    mainShader.Use();

    if ( !light.LoadShader( "lampShader.vert", "lampShader.frag" ) ) {
        exit( EXIT_FAILURE );
    }
    lampShaderProgram = light.GetShaderProgram();

    // tell the cube to use this shader
    cube.setShaderProg(program);
    cube.init();
    cube.move(glm::vec3(0.0f, -5.0f, -1.0f));
    cube.scale(glm::vec3(1.5f, 0.1f, 1.5f));

    // set cube material properties
#if 0
    GLuint cubeAmbLoc = glGetUniformLocation(program, "material.ambient");
    GLuint cubeDiffLoc = glGetUniformLocation(program, "material.diffuse");
    GLuint cubeSpecLoc = glGetUniformLocation(program, "material.specular");
    GLuint cubeShiniLoc = glGetUniformLocation(program, "material.shininess");

    glUniform3f(cubeAmbLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(cubeDiffLoc, 1.0f, 0.5f, 0.31f);
    glUniform3f(cubeSpecLoc, 0.5f, 0.5f, 0.5f);
    glUniform1f(cubeShiniLoc, 32.0f);
#endif
    init_texture();
    init_texture2();
    init_matuniform();
    init_light();

    // we're doing 3d now so we want opengl to check who's in front of who
    glEnable(GL_DEPTH_TEST);
}

void update_matrices()
{
    mainShader.SetViewMat(camera.getLookAtPtr());
    mainShader.SetProjMat(glm::value_ptr(projMat));

    light.GetShader().SetViewMat( camera.getLookAtPtr() );
    light.GetShader().SetProjMat( glm::value_ptr(projMat) );
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
    texture.Bind();

    glActiveTexture(GL_TEXTURE1);
    texture2.Bind();

    cube.draw();

    light.Draw();
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

    //Model cri_model("../crysis-nano-suit-2/source/scene.fbx");
    Model cri_model("../al.obj");
    cri_model.setShaderProg(program);
    cri_model.setModelMatLoc(modelMatLoc);
    cri_model.scale(glm::vec3(0.1f, 0.1f, 0.1f));
    cri_model.move(glm::vec3(-1.0f, -2.0f, -5.0f));


    while(!window.shouldClose())
    {

        camera.update();
        update_matrices();
        display();

        cri_model.draw();

        window.update();
    }

    return 0;
}

