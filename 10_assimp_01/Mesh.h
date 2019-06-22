#ifndef _MESH_H_INCLUDED_
#define _MESH_H_INCLUDED_

#include "Vertex.h"
#include "Texture.h"

extern "C" {
#include <GL/glew.h>
}

#include <vector>
#include <algorithm>

class Mesh
{
public:

    /* Constructor */
    Mesh(std::vector<Vertex> &vertices, 
         std::vector<unsigned int> &indices, 
         std::vector<Texture> &textures
    );

    /* Draw */
    void draw(GLuint shaderProgram);

    /* Data */
    std::vector<Texture>      m_textures;
    std::vector<Vertex>       m_vertices;

    std::vector<unsigned int> m_indices;

private:
    unsigned int m_vao; // array buffer object
    unsigned int m_vbo; // vertex buffer object
    unsigned int m_ebo; // element buffer object

    void init_mesh(void);
};

#endif // _MESH_H_INCLUDED_

