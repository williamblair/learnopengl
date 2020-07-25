#ifndef LIGHT_SOURCE_H_INCLUDED
#define LIGHT_SOURCE_H_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct LightSource {
    unsigned int VAO; // vertex array object
    unsigned int VBO; // vertex buffer object
    glm::vec3 color; // the light color
    glm::vec3 objectColor; // the color of the object representing the light
} LightSource;

// use the same vertex buffer object as the cube object
LightSource createLightSource(const Cube& cube)
{
    LightSource light;

    light.color = glm::vec3(1.0F, 1.0F, 1.0F);

    size_t floatsPerVertex = 5;
    size_t floatsPerPosition = 3;
    size_t floatsPerTexCoord = 2;

    glGenVertexArrays(1, &light.VAO);
    glBindVertexArray(light.VAO);
    // the input VBO already has the data
    light.VBO = cube.VBO;
    glBindBuffer(GL_ARRAY_BUFFER, light.VBO);

    // set object attribs
    int posAttribLocation = 0; // aPos, where we set location = 0
    int dataType = GL_FLOAT;
    int shouldNormalize = GL_FALSE;
    int vertexStride = floatsPerVertex * sizeof(GLfloat);
    void* posBeginOffset = (void*)0;
    void* texBeginOffset = (void*)(floatsPerPosition *
        sizeof(GLfloat));
    glVertexAttribPointer(posAttribLocation,
        floatsPerPosition,
        dataType,
        shouldNormalize,
        vertexStride,
        posBeginOffset);
    glEnableVertexAttribArray(posAttribLocation);

    // unbind the current buffers
    // ORDER MATTERS - the VAO must be unbinded FIRST!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return light;
}

#endif //!LIGHT_SOURCE_H_INCLUDED
