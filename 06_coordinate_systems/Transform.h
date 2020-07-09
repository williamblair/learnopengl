#ifndef TRANSFORM_H_INCLUDED
#define TRANSFORM_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

glm::mat4 createTransformationMatrix()
{
    // identity matrix starting point
    glm::mat4 transMat = glm::mat4(1.0F);

    // model matrix (model->world coordinates)
    // rotates 55 deg around X axis
    glm::mat4 modelMat = glm::mat4(1.0F);
    modelMat = glm::rotate(modelMat, 
        glm::radians(-55.0F), 
        glm::vec3(1.0F, 0.0F, 0.0F));

    // View/camera matrix (world->view coordinates)
    // move slightly backwards in the scene (move the entire world forwards)
    // OpenGL is a right handed system so world moves in the negative z axis
    // (-z axis = forwards/away, +z axis = backwards/towards)
    glm::mat4 viewMat = glm::mat4(1.0F);
    viewMat = glm::translate(viewMat, glm::vec3(0.0F, 0.0F, -3.0F));

    // Projection matrix (view->clip space coordinates)
    float FOV = 45.0F;
    float aspectRatio = 800.0F / 600.0F;
    float nearClip = 0.1F;
    float farClip = 100.0F;
    glm::mat4 projMat = glm::perspective(glm::radians(FOV),
        aspectRatio,
        nearClip,
        farClip);

    // transformations should be applied from right to left: 
    // model, view, projection
    transMat = projMat * viewMat * modelMat;

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

