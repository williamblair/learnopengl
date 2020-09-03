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
         // positions           // normals         // texture Coords    // tangent, bitangent (calculated next)
         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,          0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
        -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,          0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,          0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,

         25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  2.0f, 0.0f,          0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
        -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  0.0f, 2.0f,          0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
         25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  2.0f, 2.0f,          0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f
};

static inline void calcFloorTangentBitangent()
{
    const size_t floatsPerVertex = 14;
    for (size_t triangle = 0; triangle < 2; ++triangle)
    {
        const size_t uvOffset = 6; // pos, normal
        const size_t tangentOffset = 8; // pos, normal, texcoord
        const size_t bitangentOffset = tangentOffset + 3;
        const size_t triIndex1 = ((triangle*3)+0) * floatsPerVertex;
        const size_t triIndex2 = ((triangle*3)+1) * floatsPerVertex;
        const size_t triIndex3 = ((triangle*3)+2) * floatsPerVertex;

        glm::vec3 pos1 = glm::vec3(floorVertices[triIndex1+0], floorVertices[triIndex1+1], floorVertices[triIndex1+2]);
        glm::vec3 pos2 = glm::vec3(floorVertices[triIndex2+0], floorVertices[triIndex2+1], floorVertices[triIndex2+2]);
        glm::vec3 pos3 = glm::vec3(floorVertices[triIndex3+0], floorVertices[triIndex3+1], floorVertices[triIndex3+2]);
        
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        #define UV_INDEX(v, i) floorVertices[v+uvOffset+i]
        glm::vec2 uv1 = glm::vec2(UV_INDEX(triIndex1,0),UV_INDEX(triIndex1,1));
        glm::vec2 uv2 = glm::vec2(UV_INDEX(triIndex2,0),UV_INDEX(triIndex2,1));
        glm::vec2 uv3 = glm::vec2(UV_INDEX(triIndex3,0),UV_INDEX(triIndex3,1));
        #undef UV_INDEX

        glm::vec2 dUv1 = uv2 - uv1;
        glm::vec2 dUv2 = uv3 - uv1;

        float f = 1.0f / (dUv1.x * dUv2.y - dUv2.x * dUv1.y);

        glm::vec3 tangent(0.0f);
        tangent.x = f * (dUv2.y * edge1.x - dUv1.y * edge2.x);
        tangent.y = f * (dUv2.y * edge1.y - dUv1.y * edge2.y);
        tangent.z = f * (dUv2.y * edge1.z - dUv1.y * edge2.z);

        glm::vec3 bitangent(0.0f);
        bitangent.x = f * (-dUv2.x * edge1.x + dUv1.x * edge2.x);
        bitangent.y = f * (-dUv2.x * edge1.y + dUv1.x * edge2.y);
        bitangent.z = f * (-dUv2.x * edge1.z + dUv1.x * edge2.z);

        #define STORE_TANGENT(baseIndex) \
            floorVertices[baseIndex+tangentOffset+0] = tangent.x; \
            floorVertices[baseIndex+tangentOffset+1] = tangent.y; \
            floorVertices[baseIndex+tangentOffset+2] = tangent.z
        STORE_TANGENT(triIndex1);
        STORE_TANGENT(triIndex2);
        STORE_TANGENT(triIndex3);
        #undef STORE_TANGENT

        #define STORE_BITANGENT(baseIndex) \
            floorVertices[baseIndex+bitangentOffset+0] = bitangent.x; \
            floorVertices[baseIndex+bitangentOffset+1] = bitangent.y; \
            floorVertices[baseIndex+bitangentOffset+2] = bitangent.z
        STORE_BITANGENT(triIndex1);
        STORE_BITANGENT(triIndex2);
        STORE_BITANGENT(triIndex3);
        #undef STORE_BITANGENT
    }
}

Floor createFloor()
{
    Floor floor;

    floor.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 14;
    size_t floatsPerPosition = 3;
    size_t floatsPerTexCoord = 2;
    size_t floatsPerNormal = 3;
    size_t floatsPerTangent = 3;
    size_t floatsPerBitangent = 3;

    calcFloorTangentBitangent();

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
    int tanAttribLocation = 3; // aTangent, where we set location = 3
    int bitanAttribLocation = 4; // aBitangent, where we set location = 4
    int dataType = GL_FLOAT;
    int shouldNormalize = GL_FALSE;
    int vertexStride = floatsPerVertex * sizeof(GLfloat);
    void* posBeginOffset = (void*)0;
    void* texBeginOffset = (void*)((floatsPerPosition + floatsPerNormal) *
        sizeof(GLfloat));
    void* normalBeginOffset = (void*)((floatsPerPosition) *
        sizeof(GLfloat));
    void* tanBeginOffset = (void*)((floatsPerPosition + floatsPerNormal + floatsPerTexCoord) *
                                    sizeof(GLfloat));
    void* bitanBeginOffset = (void*)((floatsPerPosition + floatsPerNormal + floatsPerTexCoord + floatsPerTangent) *
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
    glVertexAttribPointer(tanAttribLocation,
        floatsPerTexCoord,
        dataType,
        shouldNormalize,
        vertexStride,
        tanBeginOffset);
    glEnableVertexAttribArray(tanAttribLocation);
    glVertexAttribPointer(bitanAttribLocation,
        floatsPerTexCoord,
        dataType,
        shouldNormalize,
        vertexStride,
        bitanBeginOffset);
    glEnableVertexAttribArray(bitanAttribLocation);

    // unbind the current buffers
    // ORDER MATTERS - the VAO must be unbinded FIRST!
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    return floor;
}

#endif //!FLOOR_H_INCLUDED
