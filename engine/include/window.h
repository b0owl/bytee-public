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

#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <array>

// Window creation
GLFWwindow* create_window(int x_dim, int y_dim, const char* title_bar);

// Initialization
void init_bytee();

// Default configurations
float setup_2d_orthographic(GLFWwindow *window, std::array<float, 4>);

// Window hints
void set_window_maximized();
void set_window_resizeable();
void set_window_borderless();
void set_window_disable_resizeable();

// Graphics settings presets
void apply_optimized_2d_settings();
void apply_optimized_legacy_2d_settings();

// Graphics options
void enable_msaa_4x();
void enable_msaa_8x();
void enable_double_buffering();
void set_refresh_rate(int refresh);
void set_clear_color(float R, float G, float B, float A);

// Other
void glCleanup(GLFWwindow *window);

// Per-frame utilities
// Queries the current framebuffer size, updates the 2D orthographic projection
// and viewport, and writes the dimensions and aspect ratio to the output params.
// Safe to call every frame; no-ops if the window is minimized (zero fb size).
void update_viewport(GLFWwindow* window, int* fb_w, int* fb_h, float* aspect);

// Computes delta time since the last call. Initialize *last_time = 0 before
// the loop; returns elapsed seconds and updates *last_time automatically.
float compute_delta_time(float* last_time);

// Widget areas
struct WidgetArea {
    float x1, x2, y1, y2;
};

struct WidgetInfoArea {
    float x;
    float y;
};

WidgetArea define_widget_area(const char*  texture_id, float x1, float x2, float y1, float y2);
WidgetArea get_widget_area(const char*  texture_id);
bool check_widget_hover(GLFWwindow* window, const char*  texture_id, int fb_w, int fb_h, float cur_aspect);
int check_widget_click(GLFWwindow* window, const char*  texture_id, int fb_w, int fb_h, float cur_aspect);
WidgetInfoArea optimal_widget_info_area(const char*  texture_id, int fb_w, int fb_h, float text_size, const char* font_path, const char* text);

// Text widget areas (keyed by a string id)
WidgetArea define_text_area(const char* ID, const char* font_path, const char* text, float x, float y, float size);
bool check_text_hover(GLFWwindow* window, const char* ID, int fb_w, int fb_h, float cur_aspect);
int check_text_click(GLFWwindow* window, const char* ID, int fb_w, int fb_h, float cur_aspect);


#endif // WINDOW_H