#ifndef FLOOR_H_INCLUDED
#define FLOOR_H_INCLUDED

#include <glad/glad.h>

typedef struct Floor {
    GLfloat* vertices;
    GLuint* indices;
    size_t   numVertices;
    size_t   numIndices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int EBO; // element buffer object
    int          renderMode; // GL_LINE or GL_FILL
} Floor;

// vertex data
// texture coordinates origin are the bottom left of the texture
// (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
GLfloat floorVertices[] = {
         // positions         // normals         // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f
};
Floor createFloor()
{
    Floor floor;

    floor.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 8;
    size_t floatsPerPosition = 3;
    size_t floatsPerTexCoord = 2;
    size_t floatsPerNormal = 3;

    // create GL objects and bind them
    floor.vertices = floorVertices;
    floor.indices = nullptr;
    floor.numVertices = sizeof(floorVertices) /
        sizeof(GLfloat) /
        floatsPerVertex;
    floor.numIndices = 0;
    glGenBuffers(1, &floor.VBO);
    glGenVertexArrays(1, &floor.VAO);
    glBindVertexArray(floor.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, floor.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        floor.numVertices * floatsPerVertex * sizeof(GLfloat),
        floor.vertices,
        GL_STATIC_DRAW);

    // set object attribs
    int posAttribLocation = 0; // aPos, where we set location = 0
    int normalAttribLocation = 1; // aNormal, where we set location = 1
    int texAttribLocation = 2; // aTexCoord, where we set location = 2
    int dataType = GL_FLOAT;
    int shouldNormalize = GL_FALSE;
    int vertexStride = floatsPerVertex * sizeof(GLfloat);
    void* posBeginOffset = (void*)0;
    void* texBeginOffset = (void*)((floatsPerPosition + floatsPerNormal) *
        sizeof(GLfloat));
    void* normalBeginOffset = (void*)((floatsPerPosition) *
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

    return floor;
}

#endif //!FLOOR_H_INCLUDED
