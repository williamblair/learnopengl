
#include "Camera.h"

Camera::Camera()
: m_pos(0.0f, 0.0f, 1.0f),
  m_front(0.0f, 0.0f, -1.0f),
  m_up(0.0f, 1.0f, 0.0f),
  m_speed(0.05f)
{
    m_lookAt = glm::lookAt(
        m_pos,
        m_pos + m_front,
        m_up
    );

    m_window = NULL;

    m_last_mouseX = -1.0f;
    m_last_mouseY = -1.0f;

    m_pitch = 0.0f;
    m_yaw = 0.0f;

    m_sensitivity = 0.05f;
}

Camera::Camera(glm::vec3 pos)
: m_pos(pos),
  m_front(0.0f, 0.0f, -1.0f),
  m_up(0.0f, 1.0f, 0.0f),
  m_speed(0.05f)
{
    m_lookAt = glm::lookAt(
        m_pos,
        m_pos + m_front,
        m_up
    );

    m_last_mouseX = -1.0f;
    m_last_mouseY = -1.0f;

    m_pitch = 0.0f;
    m_yaw = 0.0f;

    m_window = NULL;

    m_sensitivity = 0.05;
}

void Camera::setWindow(Window *window)
{
    m_window = window;

    //glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Camera::update(void)
{
    double mouseX, mouseY;

    /* Poll Mouse Movement */
    glfwGetCursorPos(m_window->getWindowPtr(), &mouseX, &mouseY);

    if (m_last_mouseX == -1 && m_last_mouseY == -1) {
        m_last_mouseX = mouseX;
        m_last_mouseY = mouseY;
    } 

    double xoffset = mouseX - m_last_mouseX;
    double yoffset = m_last_mouseY - mouseY;

    m_last_mouseX = mouseX;
    m_last_mouseY = mouseY;

    xoffset *= m_sensitivity;
    yoffset *= m_sensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;


    glm::vec3 front;
    front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
    front.y = sin(glm::radians(m_pitch));
    front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
    m_front = glm::normalize(front);

    /* Update the view camera */
    m_lookAt = glm::lookAt(
        m_pos,
        m_pos + m_front,
        m_up
    );

    /* Poll Key Presses */
    if (m_window->keyIsPressed(GLFW_KEY_W)) {
        m_pos += m_speed * m_front;
    }
    if (m_window->keyIsPressed(GLFW_KEY_S)) {
        m_pos -= m_speed * m_front;
    }
    if (m_window->keyIsPressed(GLFW_KEY_A)) {
        m_pos -= glm::normalize(glm::cross(m_front, m_up)) * m_speed;
    }
    if (m_window->keyIsPressed(GLFW_KEY_D)) {
        m_pos += glm::normalize(glm::cross(m_front, m_up)) * m_speed;
    }

    m_lookAt= glm::lookAt(
        m_pos,
        m_pos + m_front,
        m_up
    );
}

glm::mat4 Camera::getLookAt(void)
{
    return m_lookAt;
}

GLfloat const *Camera::getLookAtPtr(void)
{
    return glm::value_ptr(m_lookAt);
}



