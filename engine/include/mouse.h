#ifndef MOUSE_H
#define MOUSE_H

#include <GLFW/glfw3.h>

struct MouseState {
    double x, y;
    bool left_button;
    bool right_button;
    bool middle_button;
};

inline void get_mouse_state(GLFWwindow* window, MouseState& mouse) {
    glfwGetCursorPos(window, &mouse.x, &mouse.y);
    mouse.left_button   = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)   == GLFW_PRESS;
    mouse.right_button  = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)  == GLFW_PRESS;
    mouse.middle_button = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;
}

inline void screen_to_gl(double screen_x, double screen_y,
                         int window_width, int window_height,
                         float& gl_x, float& gl_y) {
    gl_x = (screen_x / window_width) * 2.0f - 1.0f;
    gl_y = 1.0f - (screen_y / window_height) * 2.0f;
}

#endif
