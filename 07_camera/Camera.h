#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct Camera {
    glm::vec3 position;
    glm::vec3 target; // where the camera is looking at
    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 right;
} Camera;

Camera createCamera()
{
    Camera camera;

    // 3 coordinates backwards
    camera.position = glm::vec3(0.0F, 0.0F, 3.0F);

    // direction = difference between position and target
    // direction actually points in the reverse direction of its target
    camera.target = glm::vec3(0.0F, 0.0F, 0.0F);
    camera.direction = glm::normalize(camera.position - camera.target);

    // create the camera's right and up axis
    glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
    camera.right = glm::normalize(glm::cross(up, camera.direction));
    camera.up = glm::cross(camera.direction, camera.right);

    return camera;
}

// assumes only position changed currently
void updateCamera(Camera& camera)
{
    // direction = difference between position and target
    // direction actually points in the reverse direction of its target
    camera.target = glm::vec3(0.0F, 0.0F, 0.0F);
    camera.direction = glm::normalize(camera.position - camera.target);

    // create the camera's right and up axis
    glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
    camera.right = glm::normalize(glm::cross(up, camera.direction));
    camera.up = glm::cross(camera.direction, camera.right);
}

#endif //!CAMERA_H_INCLUDED
