#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include <vector>

#include <glad/glad.h>

#include "Vertex.h"
#include "Texture.h"
#include "ShaderProgram.h"

class Mesh
{
public:

    Mesh() {}
    ~Mesh() {}

    void Init(const std::vector<Vertex>& vertices,
        const std::vector<GLuint>& indices,
        const std::vector<Texture>& textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        setupMesh();
    }

    void Draw(const ShaderProgram& shader)
    {
        unsigned int diffuseCount = 1;
        unsigned int specularCount = 1;
        for (size_t i = 0; i < textures.size(); i++)
        {
            // how many samplers are in the fragment shader uMaterial currently
#define NUM_MATERIAL_SAMPLERS 3
            if (i >= NUM_MATERIAL_SAMPLERS) {
                std::cout << "Mesh: texture number greater than number of samplers in fragment shader!!!" << std::endl;
            }
#undef NUM_MATERIAL_SAMPLERS

            glActiveTexture(GL_TEXTURE0 + i);

            // assumes shader sampler uniforms in this format:
            // (inside of a "uMaterial" uniform)
            // uniform sampler2D texture_diffuse1
            // uniform sampler2D texture_diffuse2
            // uniform sampler2D texture_diffuse3
            // uniform sampler2D texture_specular1
            // uniform sampler2D texture_specular2
            // uniform sampler2D texture_specular3
            // ...
            TextureType texType = textures[i].type;
#if 0
            std::string materialNumStr;
            if (texType == TextureType::Diffuse) {
                materialNumStr = "uMaterial.texture_diffuse";
                materialNumStr += std::to_string(diffuseCount++);
            }
            else if (texType == TextureType::Specular) {
                materialNumStr = "uMaterial.texture_specular";
                materialNumStr += std::to_string(specularCount++);
            }
#endif
            std::string materialNumStr = "uDiffuseTex";
            if (texType == TextureType::Specular) {
                materialNumStr = "uSpecularTex";
            }

            size_t materialLoc = glGetUniformLocation(shader.id,
                materialNumStr.c_str());
            glUniform1i(materialLoc, i);
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        // draw the mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture> textures;

private:
    GLuint VAO; // vertex array object
    GLuint VBO; // vertex buffer object
    GLuint EBO; // element buffer object

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
            &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
            &indices[0], GL_STATIC_DRAW);

        // vertex attributes
        size_t floatsPerVertex = 8;
        size_t floatsPerPosition = 3;
        size_t floatsPerNormal = 3;
        size_t floatsPerTexCoord = 2;
        int posAttribLocation = 0; // aPos, where we set location = 0
        int normalAttribLocation = 1; // aNormal, where we set location = 1
        int texAttribLocation = 2; // aTexCoord, where we set location = 2
        int dataType = GL_FLOAT;
        int shouldNormalize = GL_FALSE;
        int vertexStride = floatsPerVertex * sizeof(GLfloat);
        void* posBeginOffset = (void*)0;
        void* normalBeginOffset = (void*)(offsetof(Vertex, Normal));
        void* texBeginOffset = (void*)(offsetof(Vertex, TexCoord));
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

        // unbind the vertex array
        glBindVertexArray(0);
    }
};

#endif //! MESH_H_INCLUDED

