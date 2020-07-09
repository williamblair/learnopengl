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
GLfloat vertices[] = {
      0.5F,  0.5F, 0.0F, // top right
      0.5F, -0.5F, 0.0F, // bottom right
     -0.5F, -0.5F, 0.0F, // bottom left
     -0.5F,  0.5F, 0.0F  // top left
};
GLuint indices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};
Rectangle createRectangle()
{
    Rectangle rect;

    rect.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 3;

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
    int attribLocation = 0; // aPos, where we set location = 0
    int dataType = GL_FLOAT;
    int shouldNormalize = GL_FALSE;
    int vertexStride = floatsPerVertex * sizeof(GLfloat);
    void* beginOffset = (void*)0;
    glVertexAttribPointer(attribLocation,
        floatsPerVertex,
        dataType,
        shouldNormalize,
        vertexStride,
        beginOffset);
    glEnableVertexAttribArray(attribLocation);

    // unbind the current buffers
    // ORDER MATTERS - the VAO must be unbinded FIRST!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return rect;
}


#endif // !RECTANGLE_H_INCLUDED

