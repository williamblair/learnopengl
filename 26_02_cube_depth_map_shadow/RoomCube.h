#ifndef ROOM_CUBE_H_INCLUDED
#define ROOM_CUBE_H_INCLUDED

#include <glad/glad.h>

typedef struct RoomCube {
    GLfloat* vertices;
    GLuint*  indices;
    size_t   numVertices;
    size_t   numIndices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int EBO; // element buffer object
    int          renderMode; // GL_LINE or GL_FILL
} RoomCube;

// vertex data
// texture coordinates origin are the bottom left of the texture
// same as Cube.h except normals are negated, as we'll
// be drawing inside the room, not outside
GLfloat roomCubeVertices[] = {
    // positions             // normals           // texture coords
    -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f,
     10.0f, -10.0f, -10.0f,  0.0f,  0.0f, 1.0f,  1.0f,  0.0f,
     10.0f,  10.0f, -10.0f,  0.0f,  0.0f, 1.0f,  1.0f,  1.0f,
     10.0f,  10.0f, -10.0f,  0.0f,  0.0f, 1.0f,  1.0f,  1.0f,
    -10.0f,  10.0f, -10.0f,  0.0f,  0.0f, 1.0f,  0.0f,  1.0f,
    -10.0f, -10.0f, -10.0f,  0.0f,  0.0f, 1.0f,  0.0f,  0.0f,

    -10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,
     10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  -1.0f,  1.0f,  0.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  -1.0f,  1.0f,  1.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  -1.0f,  1.0f,  1.0f,
    -10.0f,  10.0f,  10.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  1.0f,
    -10.0f, -10.0f,  10.0f,  0.0f,  0.0f,  -1.0f,  0.0f,  0.0f,

    -10.0f,  10.0f,  10.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    -10.0f,  10.0f, -10.0f, 1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
    -10.0f, -10.0f, -10.0f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -10.0f, -10.0f, -10.0f, 1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
    -10.0f, -10.0f,  10.0f, 1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
    -10.0f,  10.0f,  10.0f, 1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

     10.0f,  10.0f,  10.0f,  -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
     10.0f,  10.0f, -10.0f,  -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
     10.0f, -10.0f, -10.0f,  -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     10.0f, -10.0f, -10.0f,  -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
     10.0f, -10.0f,  10.0f,  -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
     10.0f,  10.0f,  10.0f,  -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

    -10.0f, -10.0f, -10.0f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,
     10.0f, -10.0f, -10.0f,  0.0f, 1.0f,  0.0f,  1.0f,  1.0f,
     10.0f, -10.0f,  10.0f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
     10.0f, -10.0f,  10.0f,  0.0f, 1.0f,  0.0f,  1.0f,  0.0f,
    -10.0f, -10.0f,  10.0f,  0.0f, 1.0f,  0.0f,  0.0f,  0.0f,
    -10.0f, -10.0f, -10.0f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f,

    -10.0f,  10.0f, -10.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  1.0f,
     10.0f,  10.0f, -10.0f,  0.0f,  -1.0f,  0.0f,  1.0f,  1.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  -1.0f,  0.0f,  1.0f,  0.0f,
     10.0f,  10.0f,  10.0f,  0.0f,  -1.0f,  0.0f,  1.0f,  0.0f,
    -10.0f,  10.0f,  10.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  0.0f,
    -10.0f,  10.0f, -10.0f,  0.0f,  -1.0f,  0.0f,  0.0f,  1.0f
};

RoomCube createRoomCube()
{
    RoomCube cube;

    cube.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 8;
    size_t floatsPerPosition = 3;
    size_t floatsPerNormal = 3;
    size_t floatsPerTexCoord = 2;

    // create GL objects and bind them
    cube.vertices = roomCubeVertices;
    cube.indices = nullptr;
    cube.numVertices = sizeof(roomCubeVertices) /
        sizeof(GLfloat) /
        floatsPerVertex;
    cube.numIndices = 0;
    glGenBuffers(1, &cube.VBO);
    glGenVertexArrays(1, &cube.VAO);
    glBindVertexArray(cube.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, cube.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        cube.numVertices * floatsPerVertex * sizeof(GLfloat),
        cube.vertices,
        GL_STATIC_DRAW);

    // set object attribs
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

    return cube;
}


#endif // !ROOM_CUBE_H_INCLUDED

