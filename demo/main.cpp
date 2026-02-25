// Copyright (c) 2026- b0owl / Brennan Marx-Rennie. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to use,
// copy, modify, and distribute the Software for NON-COMMERCIAL purposes only,
// subject to the following conditions:
//
// 1. The above copyright notice and this permission notice shall be included
//    in all copies or substantial portions of the Software.
// 2. The Software may not be used for commercial purposes without prior
//    written permission from the copyright holder.
// 3. Commercial use includes, but is not limited to, selling the Software,
//    using it in a commercial product, or using it to provide commercial services.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

#include <memory>
#include <array>

#include "../engine/include/bytee.h"
#include "scene_manager.h"
#include "scenes/title_screen.h"
#include "scenes/game_scene.h"
#include "scenes/settings_scene.h"

int main() {
    init_bytee();
    set_window_maximized();
    apply_optimized_legacy_2d_settings();

    GLFWwindow* window = create_window(800, 800, "bytee demo");

    std::array<float, 4> clearcolor = {0.0f, 0.0f, 0.0f, 1.0f};
    float aspect = setup_2d_orthographic(window, clearcolor);

    float last_time  = 0.0f;
    float cur_aspect = aspect;

    SceneManager scene_manager;
    scene_manager.set(std::make_unique<TitleScreen>());

    while (!glfwWindowShouldClose(window)) {
        float delta_time = compute_delta_time(&last_time);

        int fb_w, fb_h;
        update_viewport(window, &fb_w, &fb_h, &cur_aspect);

        SceneID next = scene_manager.tick(window, delta_time, cur_aspect, fb_w, fb_h);

        if      (next == SceneID::Game)     scene_manager.set(std::make_unique<GameScene>());
        else if (next == SceneID::Title)    scene_manager.set(std::make_unique<TitleScreen>());
        else if (next == SceneID::Settings) scene_manager.set(std::make_unique<SettingsScene>());

        glCleanup(window);
    }

    glfwTerminate();
    return 0;
}
