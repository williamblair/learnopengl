#ifndef _VERTEX_H_INCLUDED_
#define _VERTEX_H_INCLUDED_

#include <glm/glm.hpp>

typedef struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec2 tex_coords;
} Vertex;


#endif // _VERTEX_H_INCLUDED_

