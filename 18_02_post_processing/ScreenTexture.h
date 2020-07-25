#ifndef SCREEN_TEXTURE_H_INCLUDED
#define SCREEN_TEXTURE_H_INCLUDED

#include <glad/glad.h>

typedef struct ScreenTexture {
    GLfloat* vertices;
    GLuint*  indices;
    size_t   numVertices;
    size_t   numIndices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int EBO; // element buffer object
    int          renderMode; // GL_LINE or GL_FILL
} ScreenTexture;

GLfloat screenTexVertices[] = {
    // positions    // texcoords
    -1.0f, 1.0f,    0.0f, 1.0f,
    -1.0f, -1.0f,   0.0f, 0.0f,
    1.0f, -1.0f,    1.0f, 0.0f,

    -1.0f, 1.0f,    0.0f, 1.0f,
    1.0f, -1.0f,    1.0f, 0.0f,
    1.0f, 1.0f,     1.0f, 1.0f
};

ScreenTexture createScreenTexture()
{
    ScreenTexture screenTexture;

    screenTexture.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 4;
    size_t floatsPerPosition = 2;
    size_t floatsPerTexCoord = 2;

    // create GL objects and bind them
    screenTexture.vertices = screenTexVertices;
    screenTexture.indices = nullptr;
    screenTexture.numVertices = sizeof(screenTexVertices) /
        sizeof(GLfloat) /
        floatsPerVertex;
    screenTexture.numIndices = 0;
    glGenBuffers(1, &screenTexture.VBO);
    glGenVertexArrays(1, &screenTexture.VAO);
    glBindVertexArray(screenTexture.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenTexture.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        screenTexture.numVertices * floatsPerVertex * sizeof(GLfloat),
        screenTexture.vertices,
        GL_STATIC_DRAW);

    // set object attribs
    int posAttribLocation = 0; // aPos, where we set location = 0
    int texAttribLocation = 1; // aTexCoord, where we set location = 1
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

    return screenTexture;
}

#endif // !SCREEN_TEXTURE_H_INCLUDED

