#ifndef _LIGHT_H_INCLUDED_
#define _LIGHT_H_INCLUDED_

#include <string>
#include <cstdio>

#include <GL/glew.h>

// matrix library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// the light object is currently a cube
#include "Cube.h"

#include "Shader.h"

/*
===============================================================================
Light

    Contains location/physical object of and display properties of a light
===============================================================================
*/

class Light
{
public:

    /*
    ===================
    Constructor/Deconstructor
    ===================
    */
    Light();
    ~Light();

    /*
    ===================
    Create Shader Program
    
        Same as Load function of Shader class but also initializes the 
        internal cube.
    ===================
    */
    bool LoadShader(const std::string &vertFile, const std::string &fragFile);

    /*
    ===================
    Set the main application shader id. Also sends the current light properties
    to said shader.
    ===================
    */
    void SetWorldShaderProgram(GLuint id);

    /*
    ===================
    Set the location of the world shader's model matrix location, for our
    internal cube to use when drawing.
    ===================
    */
    void SetModelMatLoc(GLuint id);

    /*
    ===================
    Get the shader program used by the light
    ===================
    */
    GLuint GetShaderProgram(void);

    /*
    ==================
    Get the shader wrapper object
    ==================
    */
    Shader & GetShader(void); // lets see if this works...

    /*
    ===================
    Draw
    ===================
    */
    void Draw(void);

private:
    Shader m_shader;      // shader for the light (different coloring, etc.)
    Cube   m_cube;        // physical object for light
    GLuint m_worldShader; // shader everything else uses

    /*
    ===================
    Light properties
    ===================
    */
    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
};

#endif

