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

/* 

**This file contains functions that might technically be better fit for rendering.cpp, however
** I chose to keep them here as they are tied into the widgeting system. I may move them in a 
** future update. 

*/

#include "../include/window.h"
#include "../include/mouse.h"
#include "../include/rendering.h"

#include <iostream>
#include <string>
#include <unordered_map>

#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

static bool initilized = false;

/* This function exists solely for readability, as glfwInit() + non glfw calls in the same
file may be confusing to some. Initializing through this function is NOT required but
recommended under most circumstances. */
void init_bytee() {
    glfwInit();
    initilized = true;
}

// DEFAULT CONFIGS ------------------------

/*
@brief, eliminates some boilerplate by reducing the need for
        constant GLFW calls on the users end

@param window, GLFW window
@param clrcolor, clear color
@returns aspect ratio of the window, or -1.0f on error
*/
float setup_2d_orthographic(GLFWwindow *window, std::array<float, 4> clrcolor) {
    if (!window) return -1.0f;
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    float aspect = (float)w / (float)h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect, aspect, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClearColor(clrcolor[0], clrcolor[1], clrcolor[2], clrcolor[3]);
    return aspect;
}

// WINDOW CONFIGS ------------------------

void set_window_maximized()          { glfwWindowHint(GLFW_MAXIMIZED,  GLFW_TRUE);  }
void set_window_resizeable()         { glfwWindowHint(GLFW_RESIZABLE,  GLFW_TRUE);  }
void set_window_borderless()         { glfwWindowHint(GLFW_DECORATED,  GLFW_FALSE); }
void set_window_disable_resizeable() { glfwWindowHint(GLFW_RESIZABLE,  GLFW_FALSE); }

// WINDOW CONFIGS | GRAPHICS ------------------------

/* @brief, loads a generally optimized 2d graphics configuration with modern OpenGL Core Profile */
void apply_optimized_2d_settings() {
    glfwWindowHint(GLFW_SAMPLES,                0);
    glfwWindowHint(GLFW_DOUBLEBUFFER,           GLFW_TRUE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,  3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,  3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,         GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,  GLFW_TRUE);
    glfwWindowHint(GLFW_REFRESH_RATE,           GLFW_DONT_CARE);
    glfwWindowHint(GLFW_STEREO,                 GLFW_FALSE);
    glfwWindowHint(GLFW_SRGB_CAPABLE,           GLFW_FALSE);
    glfwWindowHint(GLFW_DEPTH_BITS,             0);
    glfwWindowHint(GLFW_STENCIL_BITS,           0);
}

/* @brief, loads optimized 2d graphics settings for legacy OpenGL (glBegin/glEnd style) */
void apply_optimized_legacy_2d_settings() {
    glfwWindowHint(GLFW_SAMPLES,        0);
    glfwWindowHint(GLFW_DOUBLEBUFFER,   GLFW_TRUE);
    glfwWindowHint(GLFW_REFRESH_RATE,   GLFW_DONT_CARE);
    glfwWindowHint(GLFW_STEREO,         GLFW_FALSE);
    glfwWindowHint(GLFW_SRGB_CAPABLE,   GLFW_FALSE);
    glfwWindowHint(GLFW_DEPTH_BITS,     0);
    glfwWindowHint(GLFW_STENCIL_BITS,   0);
}

void enable_msaa_4x()                              { glfwWindowHint(GLFW_SAMPLES, 4);             }
void enable_msaa_8x()                              { glfwWindowHint(GLFW_SAMPLES, 8);             }
void enable_double_buffering()                     { glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);}
void set_refresh_rate(int refresh)                 { glfwWindowHint(GLFW_REFRESH_RATE, refresh);  }
void set_clear_color(float R, float G, float B, float A) { glClearColor(R, G, B, A);              }

/* @brief, housekeeping that runs at the end of your mainloop */
void glCleanup(GLFWwindow *window) {
    glfwSwapBuffers(window);
    glfwPollEvents();
    glClear(GL_COLOR_BUFFER_BIT);
}

/*
@brief, queries the current framebuffer size, updates the 2D orthographic
        projection and GL viewport, and writes the results to the output params.
        Call every frame instead of manually duplicating this block.
        No-ops if the window is minimized (zero framebuffer size).

@param window,  GLFW window
@param fb_w,    written with the current framebuffer width in pixels
@param fb_h,    written with the current framebuffer height in pixels
@param aspect,  written with fb_w / fb_h
*/
void update_viewport(GLFWwindow* window, int* fb_w, int* fb_h, float* aspect) {
    glfwGetFramebufferSize(window, fb_w, fb_h);
    if (*fb_w == 0 || *fb_h == 0) return;
    *aspect = (float)(*fb_w) / (float)(*fb_h);
    glViewport(0, 0, *fb_w, *fb_h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-(*aspect), *aspect, -1, 1, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

/*
@brief, computes elapsed seconds since the last call and updates *last_time.
        Initialize *last_time = 0.0f before the main loop.

@param last_time, pointer to a float tracking the previous frame's timestamp
@returns delta time in seconds
*/
float compute_delta_time(float* last_time) {
    float current = (float)glfwGetTime();
    float dt = current - *last_time;
    *last_time = current;
    return dt;
}

/* Creates a new window */
GLFWwindow* create_window(int x_dim, int y_dim, const char* title_bar) {
    if (!glfwInit()) return nullptr; 

    GLFWwindow* window = glfwCreateWindow(x_dim, y_dim, title_bar, nullptr, nullptr);
    if (!window) { glfwTerminate(); return nullptr; }

    glfwMakeContextCurrent(window);
    return window;
}

// WIDGET AREAS ------------------------

static std::unordered_map<unsigned int, WidgetArea> widget_tracker;

/*
@brief, registers a widget area for a given texture ID and returns its span.

@param texture_id, the OpenGL texture ID the area is bound to
@param x1/x2, horizontal span in world coordinates (left → right)
@param y1/y2, vertical span in world coordinates (bottom → top)
@returns the registered WidgetArea
*/
WidgetArea define_widget_area(unsigned int texture_id, float x1, float x2, float y1, float y2) {
    WidgetArea area = { x1, x2, y1, y2 };
    widget_tracker[texture_id] = area;
    return area;
}

/*
@brief, retrieves the span previously registered for a texture ID.
        Returns a zeroed WidgetArea if the ID has not been registered.

@param texture_id, the OpenGL texture ID to look up
@returns the registered WidgetArea, or { 0, 0, 0, 0 } if not found
*/
WidgetArea get_widget_area(unsigned int texture_id) {
    auto it = widget_tracker.find(texture_id);
    if (it != widget_tracker.end()) return it->second;
    return { 0.0f, 0.0f, 0.0f, 0.0f };
}

/*
@brief, checks if the mouse is hovering over a widget

@param window, pointer to a GLFWwindow
@param texture_id, ID of a loaded texture (widget)
@param fb_w/h, framebuffer size in pixels
@param cur_aspect, the framebuffer's width/height ratio
*/
bool check_widget_hover(GLFWwindow* window, unsigned int texture_id, int fb_w, int fb_h, float cur_aspect) {
    WidgetArea wa = get_widget_area(texture_id);
    MouseState ms;
    get_mouse_state(window, ms);
    float gl_x, gl_y;
    screen_to_gl(ms.x, ms.y, fb_w, fb_h, gl_x, gl_y);
    float world_x = gl_x * cur_aspect;
    float world_y = gl_y;
    return world_x >= wa.x1 && world_x <= wa.x2 &&
           world_y >= wa.y1 && world_y <= wa.y2;
}

/*
@brief, checks for mouse clicks on a widget

@params, same as previous function

@return 1, left click
@return 2, right click
@return 3, middle click
*/
int check_widget_click(GLFWwindow* window, unsigned int texture_id, int fb_w, int fb_h, float cur_aspect) {
    WidgetArea wa = get_widget_area(texture_id);
    MouseState ms;
    get_mouse_state(window, ms);
    if (check_widget_hover(window, texture_id, fb_w, fb_h, cur_aspect)) {
        if (ms.left_button) return 1;
        if (ms.right_button) return 2;
        if (ms.middle_button) return 3;
    }
    return 0;
}

/*
@brief, returns the optimal area to draw info on hover for a widget (i.e text that says 'settings')

@param texture_id, ID of a loaded texture (widget)
@param text_size, height of the text in world units (same value passed to draw_text)
@param font_path, path to the .ttf font (same value passed to draw_text)
*/
WidgetInfoArea optimal_widget_info_area(unsigned int texture_id, int fb_w, int fb_h, float text_size, const char* font_path, const char* text) {
    WidgetArea wa = get_widget_area(texture_id);
    WidgetInfoArea wia;
    float aspect = (float)fb_w / (float)fb_h;
    float text_width = get_text_width(font_path, text, text_size);
    if (aspect - wa.x2 < 0.2f) wia.x = wa.x1 - text_width - 0.01f; // place text so it ends just left of the icon
    else wia.x = wa.x2 + 0.01f;

    // draw_text renders from the baseline (y) upward by cap_height.
    // Center the text on the widget by offsetting y down by half the actual cap height.
    float cap_height = get_text_cap_height(font_path, text_size);
    wia.y = (wa.y2 + wa.y1) / 2.0f - cap_height / 2.0f;

    return wia;
}

// TEXT WIDGET AREAS ------------------------

static std::unordered_map<std::string, WidgetArea> text_area_tracker;

/*
@brief, registers a widget area for a piece of rendered text and returns its bounds.
        The bounds are computed from the text's draw position and its measured
        width/cap-height so that hover/click checks work the same way as image widgets.

@param id,        unique string key for this text area
@param font_path, path to the .ttf font (same value passed to draw_text)
@param text,      the string being rendered
@param x,         left edge in world coordinates (same x passed to draw_text)
@param y,         baseline in world coordinates (same y passed to draw_text)
@param size,      text size in world units (same value passed to draw_text)
@returns the registered WidgetArea
*/
WidgetArea define_text_area(const char* id, const char* font_path, const char* text, float x, float y, float size) {
    float cap_height = get_text_cap_height(font_path, size);
    float text_width = get_text_width(font_path, text, size);
    WidgetArea area = { x, x + text_width, y, y + cap_height };
    text_area_tracker[id] = area;
    return area;
}

/*
@brief, retrieves the span previously registered for a text id.
        Returns a zeroed WidgetArea if the id has not been registered.

@param id, the string key used in define_text_area
@returns the registered WidgetArea, or { 0, 0, 0, 0 } if not found
*/
WidgetArea get_text_area(const char* id) {
    auto it = text_area_tracker.find(id);
    if (it != text_area_tracker.end()) return it->second;
    return { 0.0f, 0.0f, 0.0f, 0.0f };
}

/*
@brief, checks if the mouse is hovering over a text widget area

@param window,     pointer to a GLFWwindow
@param id,         string key of the text area
@param fb_w/h,     framebuffer size in pixels
@param cur_aspect, the framebuffer's width/height ratio
*/
bool check_text_hover(GLFWwindow* window, const char* id, int fb_w, int fb_h, float cur_aspect) {
    WidgetArea wa = get_text_area(id);
    MouseState ms;
    get_mouse_state(window, ms);
    float gl_x, gl_y;
    screen_to_gl(ms.x, ms.y, fb_w, fb_h, gl_x, gl_y);
    float world_x = gl_x * cur_aspect;
    float world_y = gl_y;
    return world_x >= wa.x1 && world_x <= wa.x2 &&
           world_y >= wa.y1 && world_y <= wa.y2;
}

/*
@brief, checks for mouse clicks on a text widget area

@params, same as check_text_hover

@return 1, left click
@return 2, right click
@return 3, middle click
@return 0, no click
*/
int check_text_click(GLFWwindow* window, const char* id, int fb_w, int fb_h, float cur_aspect) {
    MouseState ms;
    get_mouse_state(window, ms);
    if (check_text_hover(window, id, fb_w, fb_h, cur_aspect)) {
        if (ms.left_button)   return 1;
        if (ms.right_button)  return 2;
        if (ms.middle_button) return 3;
    }
    return 0;
}