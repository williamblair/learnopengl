#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <vector>
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glad/glad.h>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"
#include "Vertex.h"

class Model
{
public:

    Model() {}
    ~Model() {}

    void Load(const std::string& filePath)
    {
        Assimp::Importer import;
        const aiScene* scene = import.ReadFile(filePath,
            aiProcess_Triangulate | aiProcess_FlipUVs);

        if (!scene || 
            scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || 
            !scene->mRootNode)
        {
            std::cout << "ASSIMP error: " << import.GetErrorString() << std::endl;
            return;
        }

        directory = filePath.substr(0, filePath.find_last_of('/'));

        processNode(scene->mRootNode, scene);
    }

    void Draw(const ShaderProgram& shader)
    {
        for (Mesh& mesh : meshes)
        {
            mesh.Draw(shader);
        }
    }


    std::vector<Mesh> meshes;
    std::string directory;

    std::vector<Texture> loaded_textures; // keep track of already loaded

private:
    void processNode(aiNode* node, const aiScene* scene)
    {
        // process node meshes, if any
        for (size_t i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(processMesh(mesh, scene));
        }

        // process the node children, if any
        for (size_t i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }
    }

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture> textures;

        for (size_t i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;

            vertex.Position.x = mesh->mVertices[i].x;
            vertex.Position.y = mesh->mVertices[i].y;
            vertex.Position.z = mesh->mVertices[i].z;

            vertex.Normal.x = mesh->mNormals[i].x;
            vertex.Normal.y = mesh->mNormals[i].y;
            vertex.Normal.z = mesh->mNormals[i].z;

            // assuming a single texture for now
            if (mesh->mTextureCoords[0])
            {
                vertex.TexCoord.x = mesh->mTextureCoords[0][i].x;
                vertex.TexCoord.y = mesh->mTextureCoords[0][i].y;
            }
            else {
                vertex.TexCoord.x = 0.0f;
                vertex.TexCoord.y = 0.0f;
            }

            vertices.push_back(vertex);
        }

        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            
            std::vector<Texture> diffuseMaps = loadMaterialTextures(material,
                    aiTextureType_DIFFUSE,
                    TextureType::Diffuse);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture> specularMaps = loadMaterialTextures(material, 
                aiTextureType_SPECULAR, TextureType::Specular);
            textures.insert(textures.end(), 
                specularMaps.begin(), 
                specularMaps.end());
        }

        Mesh myMesh;
        myMesh.Init(vertices, indices, textures);
        return myMesh;
    }

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,
        aiTextureType type, TextureType typeName)
    {
        std::vector<Texture> textures;
        for (size_t i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            std::string fullName = directory + "/" + std::string(str.C_Str());
            bool alreadyLoaded = false;
            for (auto& loadedTexture : loaded_textures)
            {
                if (loadedTexture.fileName == fullName)
                {
                    alreadyLoaded = true;
                    textures.push_back(loadedTexture);
                    break;
                }
            }
            if (!alreadyLoaded) {
                std::cout << "Model loading texture: " << fullName << std::endl;
                Texture texture = createTexture(fullName);
                textures.push_back(texture);
                loaded_textures.push_back(texture);
            }
        }

        return textures;
    }
};

#endif //!MODEL_H_INCLUDED
