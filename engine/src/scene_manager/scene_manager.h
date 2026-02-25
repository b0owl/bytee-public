#pragma once
#include "scene.h"      // for Scene, SceneID
#include <memory>
#include <GLFW/glfw3.h>

class SceneManager {
public:
    void set(std::unique_ptr<Scene> scene);
    SceneID tick(GLFWwindow* window, float delta_time,
                 float cur_aspect, int fb_w, int fb_h);
private:
    std::unique_ptr<Scene> m_scene;
};