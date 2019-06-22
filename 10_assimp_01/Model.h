#ifndef _MODEL_H_INCLUDED_
#define _MODEL_H_INCLUDED_

#include <GL/glew.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// matrix library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    void draw(void);

    /* Store the uniform location of the model matrix in the shader to send our model matrix to when drawing
     * this should be called before draw!
     */
    void setModelMatLoc(GLuint loc);

    /* set the shader program for the model to use */
    void setShaderProg(GLuint prog);

    /* Transformations */
    void rotate(GLfloat angle, glm::vec3 axis);
    void scale(glm::vec3 scale);
    void move(glm::vec3 pos);

    /* Getters */
    glm::vec3 getPos(void) const;

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

    /* Load a texture using assimp */
    GLuint load_texture(const char *filename, const char *dir);


    /* model matrix */
    glm::mat4 m_modelMat;
    
    /* shader model matrix uniform location */
    GLuint m_modelMatLoc;

    /* shader program to use for drawing */
    GLuint m_shaderProg;

    /* rotation */
    GLfloat   m_rotAngle;
    glm::vec3 m_rotAxis;

    /* translation */
    glm::vec3 m_pos;

    /* scale */
    glm::vec3 m_scale;

    /* recreates the model matrix from rotation, position, etc. */
    void updateModelMat(void);
};

#endif // _MODEL_H_INCLUDED_


