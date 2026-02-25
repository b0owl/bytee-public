#include "scene_manager.h"

void SceneManager::set(std::unique_ptr<Scene> scene) {
    m_scene = std::move(scene);
}

SceneID SceneManager::tick(GLFWwindow* window, float delta_time,
                            float cur_aspect, int fb_w, int fb_h) {
    if (!m_scene) return SceneID::None;
    return m_scene->tick(window, delta_time, cur_aspect, fb_w, fb_h);
}