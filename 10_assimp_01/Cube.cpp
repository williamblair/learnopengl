#include "Cube.h"

// constructor/deconstructor
Cube::Cube() {}
Cube::~Cube() {}

// create VAO object
bool Cube::init(void)
{
    /* 1. Create the vertex array object to hold attribs */
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

        /* 2. Create Buffer for data */
        glCreateBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    
        /* 3. Store the buffer in the data */
        glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
    
        /* 4. Set data attributes */
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, // position attributes
                12*sizeof(GLfloat), (void*)0);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, // color attributes
                12*sizeof(GLfloat), (void*)(3*sizeof(GLfloat)));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, // color attributes
                12*sizeof(GLfloat), (void*)(7*sizeof(GLfloat)));
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, // color attributes
                12*sizeof(GLfloat), (void*)(9*sizeof(GLfloat)));
        
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

    /* 5. Stop using the attrib for now,
     * Generally this isn't really necessary */
    glBindVertexArray(0);


    /* Init the cube's model matrix */
    m_modelMat = glm::mat4(1.0f); // start with identity
    m_modelMat = glm::scale(m_modelMat, glm::vec3(0.3f, 0.3f, 0.3f)); // scale the cube vertices
    m_modelMat = glm::translate(m_modelMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate cube vertices
    m_modelMat = glm::rotate(m_modelMat, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate vertices

    /* Init move vars */
    m_rotAngle = 0.0f;
    m_rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = glm::vec3(0.3f, 0.3f, 0.3f);

    return true;
}

void Cube::setModelMatLoc(GLuint loc)
{
    m_modelMatLoc = loc;
}

void Cube::setShaderProg(GLuint prog)
{
    m_shaderProg = prog;
}

void Cube::rotate(GLfloat angle, glm::vec3 axis)
{
    /* Recreate model matrix */
    m_rotAngle = angle;
    m_rotAxis  = glm::vec3(axis);
    updateModelMat();
}

void Cube::move(glm::vec3 pos)
{
    /* Recreate model matrix */
    m_pos = glm::vec3(pos);
    updateModelMat();
}

void Cube::scale(glm::vec3 scale)
{
    m_scale = glm::vec3(scale);
    updateModelMat();
}

glm::vec3 Cube::getPos(void) const
{
    return m_pos;
}

void Cube::updateModelMat(void)
{
    /* Recreate model matrix */
    m_modelMat = glm::mat4(1.0f); // start with identity
    m_modelMat = glm::scale(m_modelMat, m_scale); // scale the cube vertices
    m_modelMat = glm::translate(m_modelMat, m_pos); // translate cube vertices
    m_modelMat = glm::rotate(m_modelMat, glm::radians(m_rotAngle), m_rotAxis); // rotate vertices
}

void Cube::draw(void)
{
    /* Use the appropriate shader */
    glUseProgram(m_shaderProg);

    /* Bind the vertex array object */
    glBindVertexArray(m_vao);

    /* Send the shader the cube's model matrix */
    glUniformMatrix4fv(m_modelMatLoc, 1, GL_FALSE, glm::value_ptr(m_modelMat));

    /* Draw arrays */
    glDrawArrays(GL_TRIANGLES, 0, 36);
}


// cube vertices
// all cubes will share the same vertices
const GLfloat Cube::m_vertices[(3+4+2+3)*6*6] = {
    // position          // color                   // texture coords   // normal
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f,         0.0f, 0.0f, -1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,         0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,         0.0f, 0.0f, -1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,         0.0f, 0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,         0.0f, 0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,         0.0f, 0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,         0.0f, 0.0f,  1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,         0.0f, 0.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,         0.0f, 0.0f,  1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 1.0f,         0.0f, 0.0f,  1.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,         0.0f, 0.0f,  1.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,         0.0f, 0.0f,  1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,        -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,     1.0f, 1.0f,        -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,        -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,        -1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f,        -1.0f, 0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,        -1.0f, 0.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,        1.0f, 0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,        1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 0.0f,        1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,        1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,        0.0f, -1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,        0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,        0.0f, -1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 0.0f,        0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,        0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   0.0f, 1.0f,        0.0f, -1.0f, 0.0f,

    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,        0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f, 1.0f,   1.0f, 1.0f,        0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,        0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    1.0f, 0.0f,        0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 0.0f,        0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f,    0.0f, 1.0f,       0.0f,  1.0f, 0.0f
};


