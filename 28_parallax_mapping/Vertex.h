#ifndef VERTEX_H_INCLUDED
#define VERTEX_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
} Vertex;

#endif //!VERTEX_H_INCLUDED
