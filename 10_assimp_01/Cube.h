// matrix library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Window.h"


#ifndef _CUBE_H_INCLUDED_
#define _CUBE_H_INCLUDED_

class Cube
{
public:

    // constructor/deconstructor
    Cube();
    ~Cube();

    // create vertex array
    bool init(void);

    // Store the uniform location of the model matrix in the shader to send our model matrix to when drawing
    // this should be called before draw!
    void setModelMatLoc(GLuint loc);

    // set the shader program for the cube to use
    void setShaderProg(GLuint prog);

    // binds our vertex array object and draws arrays
    void draw(void);

    // rotate the cube, assumes angle is in degrees (we'll call glm::radians)
    void rotate(GLfloat angle, glm::vec3 axis);

    // set the cube's scale matrix
    void scale(glm::vec3 scale);

    // set/get the cube's position
    void move(glm::vec3 pos);
    glm::vec3 getPos(void) const;

private:

    // vertex array object/vertex buffer object
    GLuint m_vao;
    GLuint m_vbo;

    // texture object
    GLuint m_texture;

    // model matrix
    glm::mat4 m_modelMat;
    
    // shader model matrix uniform location
    GLuint m_modelMatLoc;

    // shader program to use for drawing
    GLuint m_shaderProg;

    // all cubes will share the same vertices
    const static GLfloat m_vertices[(3+4+2+3)*6*6];

    // rotation
    GLfloat   m_rotAngle;
    glm::vec3 m_rotAxis;

    // translation
    glm::vec3 m_pos;

    // scale
    glm::vec3 m_scale;

    // recreates the model matrix from rotation, position, etc.
    void updateModelMat(void);
};

#endif // _CUBE_H_INCLUDED_

