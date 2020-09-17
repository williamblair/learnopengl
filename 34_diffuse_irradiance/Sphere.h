#ifndef SPHERE_H_INCLUDED
#define SPHERE_H_INCLUDED

#include <cmath>
#include <glad/glad.h>

typedef struct Sphere {
    GLfloat* vertices;
    GLuint*  indices;
    size_t   numVertices;
    size_t   numIndices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int EBO; // element buffer object
    int          renderMode; // GL_LINE or GL_FILL
} Sphere;

Sphere createSphere()
{
    Sphere sphere;

    sphere.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 8;
    size_t floatsPerPosition = 3;
    size_t floatsPerNormal = 3;
    size_t floatsPerTexCoord = 2;

    const GLuint X_SEGMENTS = 64;
    const GLuint Y_SEGMENTS = 64;
    const float PI = 3.14159265359;

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> uv;
    std::vector<glm::vec3> normals;
    std::vector<GLuint> indices;
    size_t indexCount;
    std::vector<GLfloat> data;

    std::cout << "Sphere position filling" << std::endl;
    for (size_t y = 0; y <= Y_SEGMENTS; y++)
    {
        for (size_t x = 0; x <= X_SEGMENTS; x++)
        {
            float xSeg = float(x) / float(X_SEGMENTS);
            float ySeg = float(y) / float(Y_SEGMENTS);

            float xPos = cos(xSeg * 2.0 * PI) * sin(ySeg * PI);
            float yPos = cos(ySeg * PI);
            float zPos = sin(xSeg * 2.0 * PI) * sin(ySeg * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            uv.push_back(glm::vec2(xSeg, ySeg));
            normals.push_back(glm::vec3(xPos, yPos, zPos));
        }
    }
    std::cout << "Sphere index filling" << std::endl;

    for (size_t y = 0; y < Y_SEGMENTS; y++)
    {
        if (y % 2 == 0)
        {
            for (size_t x = 0; x <= X_SEGMENTS; ++x)
            {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y+1) * (X_SEGMENTS + 1) + x);
            }
        }
        else
        {
            for (int x = X_SEGMENTS; x >= 0; --x)
            {
                indices.push_back((y+1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
    }
    
    indexCount = indices.size();

    std::cout << "Sphere position filling" << std::endl;

    for (size_t i = 0; i < positions.size(); i++)
    {
        // positions
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
        // normals
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        // texcoords
            data.push_back(uv[i].x);
            data.push_back(uv[i].y);
    }

    std::cout << "Sphere GL data" << std::endl;

    // create GL objects and bind them
    sphere.vertices = data.data();
    sphere.indices = indices.data();
    sphere.numVertices = data.size() /
        floatsPerVertex;
    sphere.numIndices = indices.size();
    glGenBuffers(1, &sphere.VBO);
    glGenBuffers(1, &sphere.EBO);
    glGenVertexArrays(1, &sphere.VAO);
    glBindVertexArray(sphere.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphere.VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere.EBO);
    glBufferData(GL_ARRAY_BUFFER,
        sphere.numVertices * floatsPerVertex * sizeof(GLfloat),
        sphere.vertices,
        GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return sphere;
}


#endif // !SPHERE_H_INCLUDED

