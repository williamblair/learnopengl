#ifndef UNIFORM_BUFFER_OBJECT_H_INCLUDED
#define UNIFORM_BUFFER_OBJECT_H_INCLUDED

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct UniformBufferObject {
    unsigned int id;
    size_t bufferSize;
    size_t bindPoint;
} UniformBufferObject;

UniformBufferObject createUniformBufferObject() {

    UniformBufferObject ubo;

    glGenBuffers(1, &ubo.id);
    glBindBuffer(GL_UNIFORM_BUFFER, ubo.id);

    // enough to hold the view and projection matrices
    ubo.bufferSize = 2*sizeof(glm::mat4);
    glBufferData(GL_UNIFORM_BUFFER, ubo.bufferSize, NULL, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // entire buffer, at binding point 0
    ubo.bindPoint = 0;
    glBindBufferRange(GL_UNIFORM_BUFFER, ubo.bindPoint, ubo.id, 0, ubo.bufferSize);

    return ubo;
}

#endif // !UNIFORM_BUFFER_OBJECT_H_INCLUDED

