#include "Mesh.h"

/*
 * Constructor 
 */
Mesh::Mesh(std::vector<Vertex> &vertices, 
         std::vector<unsigned int> &indices, 
         std::vector<Texture> &textures
)
{
    /* Set data size */
    m_vertices.resize(vertices.size());
    m_indices.resize(indices.size());
    m_textures.resize(textures.size());

    /* Copy data */
    std::copy(vertices.begin(), vertices.end(), m_vertices.begin());
    std::copy(indices.begin(), indices.end(), m_indices.begin());
    std::copy(textures.begin(), textures.end(), m_textures.begin());

    /* Parse data */
    init_mesh();
}

/*
 * Helper Initialization function
 */
void Mesh::init_mesh(void)
{
    /* Create Objects */
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    /* Bind Buffers */
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);

    /* Set buffer data */
    glBufferData(GL_ARRAY_BUFFER, 
                 m_vertices.size() * sizeof(Vertex), 
                 m_vertices.data(), 
                 GL_STATIC_DRAW
    );
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
                 m_indices.size() * sizeof(unsigned int),
                 m_indices.data(),
                 GL_STATIC_DRAW
    );

    /* Vertex attribs */
    glEnableVertexAttribArray(0); // position
    glEnableVertexAttribArray(1); // normal
    glEnableVertexAttribArray(2); // texture
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (void*)(sizeof(glm::vec3)*2)
    );

    glBindVertexArray(0);
}

/*
 * Render the object
 */
void Mesh::draw(GLuint shaderProgram)
{
    unsigned int diffuse_num = 1;
    unsigned int specular_num = 1;

    /* Activate textures */
    for ( size_t i = 0; i < m_textures.size(); ++i ) 
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = m_textures[i].type;
        if (name == "texture_diffuse") {
            number = std::to_string(diffuse_num++);
        }
        else if (name == "texture_specular") {
            number = std::to_string(specular_num++);
        }

        /* Set the texture number */
        GLuint loc = glGetUniformLocation(shaderProgram, ("material."+name+number).c_str());
        glUniform1f(loc, i);

        glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    /* Draw the mesh */
    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


