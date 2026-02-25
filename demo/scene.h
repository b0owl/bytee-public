#pragma once

#include <GLFW/glfw3.h>

enum class SceneID { None, Title, Game, Settings };

struct Scene {
    virtual ~Scene() = default;
    // Returns the next scene to transition to, or SceneID::None to stay
    virtual SceneID tick(GLFWwindow* window, float delta_time,
                         float cur_aspect, int fb_w, int fb_h) = 0;
};
