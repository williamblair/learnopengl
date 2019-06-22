#ifndef _MODEL_H_INCLUDED_
#define _MODEL_H_INCLUDED_

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../stb_image.h"

#include "Mesh.h"
#include "Texture.h"

#include <vector>
#include <string>

class Model
{
public:

    /* Load a model */
    Model(const char *path);

    /* Draw the model */
    void draw(GLuint shaderProg);

private:
   
    /* Data */
    std::vector<Mesh> m_meshes;
    std::string       m_directory;

    /* Helper functions */
    void load_model(const char *path);

    void process_node(aiNode *node, const aiScene *scene);
    Mesh process_mesh(aiMesh *mesh, const aiScene *scene);

    std::vector<Texture> load_mat_textures(aiMaterial *mat, aiTextureType type, 
                                            std::string type_name
    );

    /* Load a texture using SOIL */
    GLuint load_texture(const char *filename, const char *dir);
};

#endif // _MODEL_H_INCLUDED_


