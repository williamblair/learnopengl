#ifndef RECTANGLE_H_INCLUDED
#define RECTANGLE_H_INCLUDED

#include <glad/glad.h>

typedef struct Rectangle {
    GLfloat* vertices;
    GLuint*  indices;
    size_t   numVertices;
    size_t   numIndices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int EBO; // element buffer object
    int          renderMode; // GL_LINE or GL_FILL
} Rectangle;

// vertex data
// texture coordinates origin are the bottom left of the texture
GLfloat vertices[] = {
      // positions        // colors         // Texture Coordinates
      0.5F,  0.5F, 0.0F,  1.0F, 0.0F, 0.0F,  1.0F, 1.0F, // top right
      0.5F, -0.5F, 0.0F,  0.0F, 1.0F, 0.0F,  1.0F, 0.0F, // bottom right
     -0.5F, -0.5F, 0.0F,  0.0F, 0.0F, 1.0F,  0.0F, 0.0F, // bottom left
     -0.5F,  0.5F, 0.0F,  1.0F, 0.0F, 1.0F,  0.0F, 1.0F  // top left
};
GLuint indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};
Rectangle createRectangle()
{
    Rectangle rect;

    rect.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 8;
    size_t floatsPerPosition = 3;
    size_t floatsPerColor = 3;
    size_t floatsPerTexCoord = 2;

    // create GL objects and bind them
    rect.vertices = vertices;
    rect.indices = indices;
    rect.numVertices = sizeof(vertices) /
        sizeof(GLfloat) /
        floatsPerVertex;
    rect.numIndices = sizeof(indices) /
                     sizeof(GLuint);
    glGenBuffers(1, &rect.VBO);
    glGenVertexArrays(1, &rect.VAO);
    glGenBuffers(1, &rect.EBO);
    glBindVertexArray(rect.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, rect.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        rect.numVertices * floatsPerVertex * sizeof(GLfloat),
        rect.vertices,
        GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rect.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        rect.numIndices * sizeof(GLuint),
        rect.indices,
        GL_STATIC_DRAW);

    // set object attribs
    int posAttribLocation = 0; // aPos, where we set location = 0
    int colAttribLocation = 1; // aColor, where we set location = 1
    int texAttribLocation = 2; // aTexCoord, where we set location = 2
    int dataType = GL_FLOAT;
    int shouldNormalize = GL_FALSE;
    int vertexStride = floatsPerVertex * sizeof(GLfloat);
    void* posBeginOffset = (void*)0;
    void* colBeginOffset = (void*)(floatsPerPosition * sizeof(GLfloat));
    void* texBeginOffset = (void*)((floatsPerPosition + floatsPerColor) *
                                    sizeof(GLfloat));
    glVertexAttribPointer(posAttribLocation,
        floatsPerPosition,
        dataType,
        shouldNormalize,
        vertexStride,
        posBeginOffset);
    glEnableVertexAttribArray(posAttribLocation);
    glVertexAttribPointer(colAttribLocation,
        floatsPerColor,
        dataType, 
        shouldNormalize,
        vertexStride,
        colBeginOffset);
    glEnableVertexAttribArray(colAttribLocation);
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return rect;
}


#endif // !RECTANGLE_H_INCLUDED

