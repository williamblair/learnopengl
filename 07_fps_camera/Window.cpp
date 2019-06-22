#include "Window.h"

Window::Window()
{}

Window::~Window()
{
    if (m_window != NULL)
        glfwDestroyWindow(m_window);

    glfwTerminate();
}


void Window::m_error_callback(int error, const char *description)
{
    std::cout << "GLFW error: " << error << std::endl;
    std::cout << "GLFW error: " << description << std::endl;
}

bool Window::init(int width, int height, const char *title)
{
    if (!glfwInit()) {
        std::cout << "Error initializing glfw!\n";
        return false;
    }

    // MAC-SPECIFIC: Convince Mac that, yes, we want to use OpenGL version 4.3 (and NOT 2.1)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    glfwSetErrorCallback(Window::m_error_callback);

    m_window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!m_window) {
        std::cout << "Failed to create glfw window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    GLenum err = glewInit();

    glGetError();

    if (err != GLEW_OK) {
        std::cout << "Failed to init glew: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        return false;
    }

    // otherwise show glew version
    std::cout << "GLEW initialized, version: " << glewGetString(GLEW_VERSION) << std::endl;

    return true;
}

void Window::update(void)
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

bool Window::shouldClose(void)
{
    return (bool)glfwWindowShouldClose(m_window);
}


void Window::setKeyHandler(GLFWkeyfun cbfun)
{
    glfwSetKeyCallback(m_window, cbfun);
}


void Window::setMouseHandler(GLFWcursorposfun cbfun)
{
    glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(m_window, cbfun);
}

void Window::setScrollHandler(GLFWscrollfun cbfun)
{
    glfwSetScrollCallback(m_window, cbfun);
}

bool Window::keyIsPressed(int key)
{
    return (glfwGetKey(m_window, key) == GLFW_PRESS);
}


