#include "Model.h"
#include <cstring>

/*
 * Constructor
 */
Model::Model(const char *path)
{
    /* Load the data with assimp */
    printf("Before model load\n");
    load_model(path);

    /* Init the model matrix */
    m_modelMat = glm::mat4(1.0f); // start with identity
    m_modelMat = glm::scale(m_modelMat, glm::vec3(1.0f, 1.0f, 1.0f)); // scale the cube vertices
    m_modelMat = glm::translate(m_modelMat, glm::vec3(0.0f, 0.0f, 0.0f)); // translate cube vertices
    m_modelMat = glm::rotate(m_modelMat, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // rotate vertices

    /* Init transformation vars */
    m_rotAngle = 0.0f;
    m_rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
    m_pos = glm::vec3(0.0f, 0.0f, 0.0f);
    m_scale = glm::vec3(1.0f, 1.0f, 1.0f);

    updateModelMat();
}

void Model::updateModelMat(void)
{
    /* Recreate model matrix */
    m_modelMat = glm::mat4(1.0f); // start with identity
    m_modelMat = glm::scale(m_modelMat, m_scale); // scale the cube vertices
    m_modelMat = glm::translate(m_modelMat, m_pos); // translate cube vertices
    m_modelMat = glm::rotate(m_modelMat, glm::radians(m_rotAngle), m_rotAxis); // rotate vertices
}

/*
 * Setters
 */
void Model::setModelMatLoc(GLuint loc)
{
    m_modelMatLoc = loc;
}
void Model::setShaderProg(GLuint prog)
{
    m_shaderProg = prog;
}

/*
 * Getters
 */
glm::vec3 Model::getPos(void) const
{
    return m_pos;
}

/*
 * Transformations
 */
void Model::rotate(GLfloat angle, glm::vec3 axis)
{
    /* Recreate model matrix */
    m_rotAngle = angle;
    m_rotAxis  = glm::vec3(axis);
    updateModelMat();
}

void Model::move(glm::vec3 pos)
{
    /* Recreate model matrix */
    m_pos = glm::vec3(pos);
    updateModelMat();
}

void Model::scale(glm::vec3 scale)
{
    m_scale = glm::vec3(scale);
    updateModelMat();
}


/*
 * Draw the model
 */
void Model::draw(void)
{
    /* Tell openGL to use the associated shader */
    glUseProgram(m_shaderProg);

    /* Send the shader the model matrix */
    glUniformMatrix4fv(m_modelMatLoc, 1, GL_FALSE, glm::value_ptr(m_modelMat));

    /* Draw each mesh */
    for (size_t i=0; i<m_meshes.size(); ++i) {
        m_meshes[i].draw(m_shaderProg);
    }
}

/*
 * Load the model
 */
void Model::load_model(const char *path)
{
    /* Create and load the assimp scene */
    Assimp::Importer importer;

    printf("Before read scene\n");

    const aiScene *scene = importer.ReadFile(
        path, 
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals // generate normals if not given too
    );

    printf("Read scene\n");

    /* Error check */
    if (!scene || 
        scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || 
        !scene->mRootNode) 
    {
        fprintf(stderr, "Error:assimp: %s\n", importer.GetErrorString());
        return;
    }
    
    /* Set the model's located directory */
    std::string dir(path);
    m_directory = dir.substr(0, dir.find_last_of('/'));

    printf("Before process node\n");

    /* Handle the resulting scene */
    process_node(scene->mRootNode, scene);

    printf("After process node\n");
}

/*
 * Handle an assimp node
 */
void Model::process_node(aiNode *node, const aiScene *scene)
{
    /* Handle all meshes */
    //printf("Node num meshes: %d\n", node->mNumMeshes);
    for ( size_t i = 0; i < node->mNumMeshes; ++i ) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(process_mesh(mesh, scene));
    }

    /* Handle the node's child meshes */
    //printf("Node num children: %d\n", node->mNumChildren);
    for ( size_t i = 0; i < node->mNumChildren; ++i ) {
        process_node(node->mChildren[i], scene);
    }
}

/*
 * Handle an assimp mesh
 */
Mesh Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    /* Load vertices */
    for ( size_t i = 0; i < mesh->mNumVertices; ++i ) {
        Vertex vertex;

        /* Position */
        vertex.pos.x = mesh->mVertices[i].x;
        vertex.pos.y = mesh->mVertices[i].y;
        vertex.pos.z = mesh->mVertices[i].z;

        /* Normal */
        vertex.normal.x = mesh->mNormals[i].x;
        vertex.normal.y = mesh->mNormals[i].y;
        vertex.normal.z = mesh->mNormals[i].z;

        /* Texture */
        if (mesh->mTextureCoords[0]) {
            vertex.tex_coords.x = mesh->mTextureCoords[0][i].x;
            vertex.tex_coords.y = mesh->mTextureCoords[0][i].y;
        } else {
            vertex.tex_coords.x = 0.0f;
            vertex.tex_coords.y = 0.0f;
        }

        vertices.push_back(vertex);
    }
    
    /* Load indices */
    for ( size_t i = 0; i < mesh->mNumFaces; ++i ) {
        aiFace face = mesh->mFaces[i];

        for ( size_t j = 0; j < face.mNumIndices; ++j ) {
            indices.push_back( face.mIndices[j] );
        }
    }

    /* Load textures */
    if ( mesh->mMaterialIndex >= 0 ) {

        //printf("Material index >= 0!\n");
        
        /* Grab the scene material pointer based on mesh mat index */
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        /* Load diffuse texture maps */
        std::vector<Texture> diffuse_maps = load_mat_textures(
            material, 
            aiTextureType_DIFFUSE, 
            "texture_diffuse"
        );
        textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

        /* Load specular texture maps */
        std::vector<Texture> specular_maps = load_mat_textures(
            material,
            aiTextureType_SPECULAR,
            "texture_specular"
        );
        textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
    }

    return Mesh(vertices, indices, textures);
}

/*
 * Load textures from an assimp node 
 */
std::vector<Texture> Model::load_mat_textures(aiMaterial *mat, aiTextureType type, 
                                            std::string type_name
)
{
    std::vector<Texture> textures;

    for ( size_t i = 0; i < mat->GetTextureCount(type); ++i) {
        
        aiString str;
        mat->GetTexture(type, i, &str);

        printf("Met get texture string: %s\n", str.data);
        printf("strlen: %d\n", strlen(str.data));

        Texture texture;
        texture.id = strlen(str.data) > 4 ? load_texture(str.data, m_directory.c_str()) : -1;
        texture.type = type_name;
        texture.path = std::string(str.data);
        textures.push_back(texture);
    }

    return textures;
}

/*
 * Load a texture using STB into GL and assign it an ID
 */
GLuint Model::load_texture(const char *filename, const char *dir)
{
    GLuint texture;

    printf("Filename: %s\n", filename);

    std::string img_name(filename);
#if 0
    std::size_t pos = img_name.find_last_of("\\");
    if (pos != std::string::npos) {
        img_name = img_name.substr(pos+1);
    } else {
        pos = img_name.find_last_of("/");
        if (pos != std::string::npos) {
            img_name = img_name.substr(pos+1);
        }
    }
#endif
    std::string fullpath = std::string(dir)+"/"+img_name;
    
    /* 1. Get the texture data from an image */
    int width, height, nChannels;
    unsigned char *data = stbi_load(fullpath.c_str(), &width, &height, &nChannels, 0);
    if (data == NULL) {
        fprintf(stderr, "Error loading texture: %s\n", fullpath.c_str());
        exit(EXIT_FAILURE);
    }

    /* 2. Generate an opengl texture */
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    /* Set filtering parameters (optional?) */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* 3. Generate the texture image from texture data */
    GLenum format;
    if (nChannels == 1) format = GL_RED;
    if (nChannels == 3) format = GL_RGB;
    if (nChannels == 4) format = GL_RGBA;
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    /* 4. Free data if necessary */
    stbi_image_free(data);

    /* Bind the texture to a sampler 
     * In this case GL_TEXTURE0 */
    /*glUniform1i(glGetUniformLocation(program, "myTexture"), 0);*/

    return texture;
}



