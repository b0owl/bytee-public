#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <GLFW/glfw3.h>

// Simple key query helpers — use these instead of calling glfwGetKey directly
inline bool is_key_down(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) == GLFW_PRESS;
}

inline bool is_key_up(GLFWwindow* window, int key) {
    return glfwGetKey(window, key) != GLFW_PRESS;
}

// Keyboard state - define storage in exactly one .cpp file
// by defining KEYBOARD_IMPL before including this header
#ifdef KEYBOARD_IMPL
int kb_key = 0;
int kb_action = 0;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    kb_key = key;
    kb_action = action;
}
#else
extern int kb_key;
extern int kb_action;
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif

#endif
