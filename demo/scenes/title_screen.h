#pragma once

#include "../../engine/src/scene_manager/scene_manager.h"
#include "../../engine/include/bytee.h"
#include "../objects.h"
#include <vector>

class TitleScreen : public Scene {
public:
    TitleScreen();
    SceneID tick(GLFWwindow* window, float delta_time,
                 float cur_aspect, int fb_w, int fb_h) override;

private:
    StaticAllocator sa;
    std::vector<DrawData*> decorations;

    float anim_time        = 0.0f;
    bool  enter_was_down   = false;
    bool  up_was_down      = false;
    bool  down_was_down    = false;
    int   selected         = 0;  // 0 = PLAY, 1 = SETTINGS
};
