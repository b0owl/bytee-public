#pragma once

#include "../../engine/src/scene_manager/scene_manager.h"
#include "../../engine/include/bytee.h"

class SettingsScene : public Scene {
public:
    SceneID tick(GLFWwindow* window, float delta_time,
                 float cur_aspect, int fb_w, int fb_h) override;

private:
    bool prev_mouse_down = false;
};
