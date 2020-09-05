#ifndef TANGENT_PLANE_H_INCLUDED
#define TANGENT_PLANE_H_INCLUDED

#include <glad/glad.h>

typedef struct TangentPlane {
    GLfloat* vertices;
    GLuint*  indices;
    size_t   numVertices;
    size_t   numIndices;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int EBO; // element buffer object
    int          renderMode; // GL_LINE or GL_FILL
} TangentPlane;

// vertex data
// texture coordinates origin are the bottom left of the texture
// tangent, bitangent calculated, so just fill with zeros for now
// 
//    // positions
//    glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
//    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
//    glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
//    glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
//    // texture coordinates
//    glm::vec2 uv1(0.0f, 1.0f);
//    glm::vec2 uv2(0.0f, 0.0f);
//    glm::vec2 uv3(1.0f, 0.0f);
//    glm::vec2 uv4(1.0f, 1.0f);
//    // normal vector
//    glm::vec3 nm(0.0f, 0.0f, 1.0f);
GLfloat tangentPlaneVertices[] = {
    // positions          // normals           // texture coords    // tangent          // bitangent
    -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,         0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
    -1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,         0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
     1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,         0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,

    -1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,         0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
     1.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,         0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
     1.0f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,         0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,
};

#if 0
static inline void calcPlaneTangentBitangent()
{
    // positions
    glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
    glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
    glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
    glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
    // texture coordinates
    glm::vec2 uv1(0.0f, 1.0f);
    glm::vec2 uv2(0.0f, 0.0f);
    glm::vec2 uv3(1.0f, 0.0f);
    glm::vec2 uv4(1.0f, 1.0f);
    // normal vector
    glm::vec3 nm(0.0f, 0.0f, 1.0f);

    // calculate tangent/bitangent vectors of both triangles
    glm::vec3 tangent1, bitangent1;
    glm::vec3 tangent2, bitangent2;
    // triangle 1
    // ----------
    glm::vec3 edge1 = pos2 - pos1;
    glm::vec3 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv2 - uv1;
    glm::vec2 deltaUV2 = uv3 - uv1;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent1 = glm::normalize(tangent1);

    bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent1 = glm::normalize(bitangent1);

    // triangle 2
    // ----------
    edge1 = pos3 - pos1;
    edge2 = pos4 - pos1;
    deltaUV1 = uv3 - uv1;
    deltaUV2 = uv4 - uv1;

    f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent2 = glm::normalize(tangent2);


    bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent2 = glm::normalize(bitangent2);


    float quadVertices[] = {
        // positions            // normal         // texcoords  // tangent                          // bitangent
        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

        pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
        pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
    };

    // 14 * 6 = 84
    std::copy(&quadVertices[0], &quadVertices[84], tangentPlaneVertices);
}
#endif

#if 1
static inline void calcPlaneTangentBitangent()
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

        glm::vec3 pos1 = glm::vec3(tangentPlaneVertices[triIndex1+0], tangentPlaneVertices[triIndex1+1], tangentPlaneVertices[triIndex1+2]);
        glm::vec3 pos2 = glm::vec3(tangentPlaneVertices[triIndex2+0], tangentPlaneVertices[triIndex2+1], tangentPlaneVertices[triIndex2+2]);
        glm::vec3 pos3 = glm::vec3(tangentPlaneVertices[triIndex3+0], tangentPlaneVertices[triIndex3+1], tangentPlaneVertices[triIndex3+2]);
        
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;

        #define UV_INDEX(v, i) tangentPlaneVertices[v+uvOffset+i]
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
            tangentPlaneVertices[baseIndex+tangentOffset+0] = tangent.x; \
            tangentPlaneVertices[baseIndex+tangentOffset+1] = tangent.y; \
            tangentPlaneVertices[baseIndex+tangentOffset+2] = tangent.z
        STORE_TANGENT(triIndex1);
        STORE_TANGENT(triIndex2);
        STORE_TANGENT(triIndex3);
        #undef STORE_TANGENT

        #define STORE_BITANGENT(baseIndex) \
            tangentPlaneVertices[baseIndex+bitangentOffset+0] = bitangent.x; \
            tangentPlaneVertices[baseIndex+bitangentOffset+1] = bitangent.y; \
            tangentPlaneVertices[baseIndex+bitangentOffset+2] = bitangent.z
        STORE_BITANGENT(triIndex1);
        STORE_BITANGENT(triIndex2);
        STORE_BITANGENT(triIndex3);
        #undef STORE_BITANGENT
    }
}
#endif

TangentPlane createTangentPlane()
{
    TangentPlane tangentPlane;

    tangentPlane.renderMode = GL_FILL; // default drawing mode
    size_t floatsPerVertex = 14;
    size_t floatsPerPosition = 3;
    size_t floatsPerNormal = 3;
    size_t floatsPerTexCoord = 2;
    size_t floatsPerTangent = 3;
    size_t floatsPerBitangent = 3;

    calcPlaneTangentBitangent();

    // create GL objects and bind them
    tangentPlane.vertices = tangentPlaneVertices;
    tangentPlane.indices = nullptr;
    tangentPlane.numVertices = sizeof(tangentPlaneVertices) /
        sizeof(GLfloat) /
        floatsPerVertex;
    tangentPlane.numIndices = 0;
    glGenBuffers(1, &tangentPlane.VBO);
    glGenVertexArrays(1, &tangentPlane.VAO);
    glBindVertexArray(tangentPlane.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, tangentPlane.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        tangentPlane.numVertices * floatsPerVertex * sizeof(GLfloat),
        tangentPlane.vertices,
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
    void* normalBeginOffset = (void*)(floatsPerPosition *
                                    sizeof(GLfloat));
    void* texBeginOffset = (void*)((floatsPerPosition + floatsPerNormal) *
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

    return tangentPlane;
}


#endif // !TANGENT_PLANE_H_INCLUDED

