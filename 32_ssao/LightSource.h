#ifndef LIGHT_SOURCE_H_INCLUDED
#define LIGHT_SOURCE_H_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct LightSource {
    unsigned int VAO; // vertex array object
    unsigned int VBO; // vertex buffer object
    glm::vec3 objectColor; // the color of the object representing the light
} LightSource;

// use the same vertex buffer object as the cube object
LightSource createLightSource(const Cube& cube)
{
    LightSource light;
   
    glGenVertexArrays(1, &light.VAO);
    glBindVertexArray(light.VAO);
    // the input VBO already has the data
    light.VBO = cube.VBO;
    glBindBuffer(GL_ARRAY_BUFFER, light.VBO);

    // set object attribs
    size_t floatsPerVertex = 14;
    size_t floatsPerPosition = 3;
    size_t floatsPerNormal = 3;
    size_t floatsPerTexCoord = 2;
    size_t floatsPerTangent = 3;
    size_t floatsPerBitangent = 3;

    int posAttribLocation = 0; // aPos, where we set location = 0
    int normalAttribLocation = 1; // aNormal, where we set location = 1
    int texAttribLocation = 2; // aTexCoord, where we set location = 2
    int dataType = GL_FLOAT;
    int shouldNormalize = GL_FALSE;
    int vertexStride = floatsPerVertex * sizeof(GLfloat);
    void* posBeginOffset = (void*)0;
    void* normalBeginOffset = (void*)(floatsPerPosition *
        sizeof(GLfloat));
    void* texBeginOffset = (void*)((floatsPerPosition + floatsPerNormal) *
        sizeof(GLfloat));
    glVertexAttribPointer(posAttribLocation,
        floatsPerPosition,
        dataType,
        shouldNormalize,
        vertexStride,
        posBeginOffset);
    glEnableVertexAttribArray(posAttribLocation);
    glVertexAttribPointer(normalAttribLocation,
        floatsPerNormal,
        dataType,
        shouldNormalize,
        vertexStride,
        normalBeginOffset);
    glEnableVertexAttribArray(normalAttribLocation);
    glVertexAttribPointer(texAttribLocation,
        floatsPerTexCoord,
        dataType,
        shouldNormalize,
        vertexStride,
        texBeginOffset);
    glEnableVertexAttribArray(texAttribLocation);

    // unbind the current buffers
    // ORDER MATTERS - the VAO must be unbinded FIRST!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return light;
}

#endif //!LIGHT_SOURCE_H_INCLUDED
