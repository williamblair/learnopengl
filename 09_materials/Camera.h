#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef _CAMERA_H_INCLUDED_
#define _CAMERA_H_INCLuDED_

class Camera
{
public:

    Camera();
    Camera(glm::vec3 pos);

    /* Set the window for the camera to check for events from */
    void setWindow(Window *window);

    /* Move the camera based on input from the window */
    void update();

    /* Get the lookAt matrix */
    glm::mat4 getLookAt(void);
    GLfloat const *getLookAtPtr(void);

    /* Get position */
    glm::vec3 getPos(void);
    GLfloat  *getPosPtr(void);

private:
    
    glm::vec3 m_pos;
    glm::vec3 m_front;
    glm::vec3 m_up;

    glm::mat4 m_lookAt;

    double m_pitch, m_yaw;
    double m_last_mouseX;
    double m_last_mouseY;
    double m_sensitivity;

    Window *m_window;

    GLfloat m_speed; // how fast the camera moves
};

#endif // _CAMERA_H_INCLUDED_

