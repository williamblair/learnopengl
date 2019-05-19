#ifndef _WINDOW_H_
#define _WINDOW_H_

extern "C" {
#include <GL/glew.h>
#include <GLFW/glfw3.h>
}

#include <iostream>

class Window
{
public:
    Window();
    ~Window();

    bool init(int width, int height, const char *title);

    void update(void);
    bool shouldClose(void);

    void setKeyHandler(GLFWkeyfun cbfun);

private:
    // Variables
    GLFWwindow *m_window = NULL;

    // helper functions
    static void m_error_callback(int error, const char *description);
};

#endif


