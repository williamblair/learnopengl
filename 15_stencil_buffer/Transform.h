#ifndef TRANSFORM_H_INCLUDED
#define TRANSFORM_H_INCLUDED

#include "Camera.h"

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

void updateTransformationMatrix(glm::mat4& transMat, 
                                const glm::vec3& objectPosition,
                                const Camera& camera)
{
    // identity matrix starting point
    transMat = glm::mat4(1.0F);

    // model matrix (model->world coordinates)
    glm::mat4 modelMat = glm::mat4(1.0F);
    // rotates 55 deg around X axis
    //modelMat = glm::rotate(modelMat,
    //    glm::radians(-55.0F),
    //    glm::vec3(1.0F, 0.0F, 0.0F));
    modelMat = glm::translate(modelMat, objectPosition);

    // View/camera matrix (world->view coordinates)
    // OpenGL is a right handed system so world moves in the negative z axis
    // (-z axis = forwards/away, +z axis = backwards/towards)
    glm::mat4 viewMat = glm::lookAt(
        camera.position,
        camera.position + camera.front,
        camera.up);

    // Projection matrix (view->clip space coordinates)
    float aspectRatio = 800.0F / 600.0F;
    float nearClip = 0.1F;
    float farClip = 100.0F;
    glm::mat4 projMat = glm::perspective(glm::radians(camera.FOV),
        aspectRatio,
        nearClip,
        farClip);

    // transformations should be applied from right to left: 
    // model, view, projection
    transMat = projMat * viewMat * modelMat;
}

void updateTransformationMatrix(glm::mat4& transMat,
    const glm::vec3& objectPosition,
    const glm::vec3& objectScale,
    const Camera& camera)
{
    // identity matrix starting point
    transMat = glm::mat4(1.0F);

    // model matrix (model->world coordinates)
    glm::mat4 modelMat = glm::mat4(1.0F);
    // rotates 55 deg around X axis
    //modelMat = glm::rotate(modelMat,
    //    glm::radians(-55.0F),
    //    glm::vec3(1.0F, 0.0F, 0.0F));
    modelMat = glm::translate(modelMat, objectPosition);
    modelMat = glm::scale(modelMat, objectScale);

    // View/camera matrix (world->view coordinates)
    // OpenGL is a right handed system so world moves in the negative z axis
    // (-z axis = forwards/away, +z axis = backwards/towards)
    glm::mat4 viewMat = glm::lookAt(
        camera.position,
        camera.position + camera.front,
        camera.up);

    // Projection matrix (view->clip space coordinates)
    float aspectRatio = 800.0F / 600.0F;
    float nearClip = 0.1F;
    float farClip = 100.0F;
    glm::mat4 projMat = glm::perspective(glm::radians(camera.FOV),
        aspectRatio,
        nearClip,
        farClip);

    // transformations should be applied from right to left: 
    // model, view, projection
    transMat = projMat * viewMat * modelMat;
}

#endif // !TRANSFORM_H_INCLUDED

