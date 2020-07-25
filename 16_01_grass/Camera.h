#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    float pitch;
    float yaw;
    float FOV;
} Camera;

Camera createCamera()
{
    Camera camera;

    // a yaw of 0 points to the positive X axis;
    // we instead want it to point towards -z
    camera.yaw = -90.0F;
    camera.pitch = 0.0F;

    camera.FOV = 45.0F;

    // 3 coordinates backwards
    camera.position = glm::vec3(0.0F, 0.0F, 3.0F);

    // -z axis = forwards
    camera.front = glm::vec3(0.0F, 0.0F, -1.0F);

    // vertical Y = up
    camera.up = glm::vec3(0.0F, 1.0F, 0.0F);

    return camera;
}

#endif //!CAMERA_H_INCLUDED
