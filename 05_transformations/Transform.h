#ifndef TRANSFORM_H_INCLUDED
#define TRANSFORM_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 createTransformationMatrix()
{
    // identity matrix starting point
    glm::mat4 transMat = glm::mat4(1.0F);
    // 90 degrees around z axis
    transMat = glm::rotate(transMat, glm::radians(90.0F), 
        glm::vec3(0.0F, 0.0F, 1.0F));
    // scale half the size
    transMat = glm::scale(transMat, glm::vec3(0.5F, 0.5F, 0.5F));

    return transMat;
}

void updateTransformationMatrix(glm::mat4& transMat, float time)
{
    transMat = glm::mat4(1.0F);

    // move right-down
    transMat = glm::translate(transMat, glm::vec3(0.5F, -0.5F, 0.0F));

    // rotate around Z axis by time
    transMat = glm::rotate(transMat, time, glm::vec3(0.0F, 0.0F, 1.0F));
}

#endif // !TRANSFORM_H_INCLUDED

