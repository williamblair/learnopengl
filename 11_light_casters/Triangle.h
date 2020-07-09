#ifndef TRIANGLE_H_INCLUDED
#define TRIANGLE_H_INCLUDED

#include <glad/glad.h>

typedef struct Triangle {
    GLfloat* vertices;
    size_t   numVertices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
} Triangle;

// vertex data
GLfloat vertices[] = {
    -0.5F, -0.5F, 0.0F,
     0.5F, -0.5F, 0.0F,
     0.0F,  0.5F, 0.0F
};
Triangle createTriangle() 
{
    Triangle tri;
    
    size_t floatsPerVertex = 3;

    // create GL objects and bind them
    tri.vertices = vertices;
    tri.numVertices = sizeof(vertices) / 
                      sizeof(GLfloat) / 
                      floatsPerVertex;
    glGenBuffers(1, &tri.VBO);
    glGenVertexArrays(1, &tri.VAO);
    glBindVertexArray(tri.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tri.VBO);
    glBufferData(GL_ARRAY_BUFFER,
                tri.numVertices * floatsPerVertex * sizeof(GLfloat),
                tri.vertices,
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

    // unbind the current array buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return tri;
}

#endif // !TRIANGLE_H_INCLUDED

