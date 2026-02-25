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

#include "objects.h"

static inline float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

static void draw_block_3d(DrawData* data) {
    const float H = 0.05f;       // half block size
    const float BD = 0.003f;     // border (outline) thickness
    const float BV = 0.006f;     // bevel thickness
    float ox = data->x;
    float oy = data->y;
    float r = data->r, g = data->g, b = data->b;

    // 1. Dark border outline
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(r * 0.2f, g * 0.2f, b * 0.2f);
        glVertex2f(ox - H,      oy - H);
        glVertex2f(ox + H,      oy - H);
        glVertex2f(ox + H,      oy + H);
        glVertex2f(ox - H,      oy + H);
    glEnd();

    // 2. Highlight layer (top-left bevel)
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(clampf(r * 1.4f, 0, 1), clampf(g * 1.4f, 0, 1), clampf(b * 1.4f, 0, 1));
        glVertex2f(ox - H + BD, oy - H + BD);
        glVertex2f(ox + H - BD, oy - H + BD);
        glVertex2f(ox + H - BD, oy + H - BD);
        glVertex2f(ox - H + BD, oy + H - BD);
    glEnd();

    // 3. Shadow layer (bottom-right bevel)
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(r * 0.45f, g * 0.45f, b * 0.45f);
        glVertex2f(ox - H + BD + BV, oy - H + BD);
        glVertex2f(ox + H - BD,      oy - H + BD);
        glVertex2f(ox + H - BD,      oy + H - BD - BV);
        glVertex2f(ox - H + BD + BV, oy + H - BD - BV);
    glEnd();

    // 4. Face (center)
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(r, g, b);
        glVertex2f(ox - H + BD + BV, oy - H + BD + BV);
        glVertex2f(ox + H - BD - BV, oy - H + BD + BV);
        glVertex2f(ox + H - BD - BV, oy + H - BD - BV);
        glVertex2f(ox - H + BD + BV, oy + H - BD - BV);
    glEnd();
}

// Renders an array of DrawData*, applying the 3D bevel effect to standard
// full-size tetris blocks (0.1x0.1) and falling back to plain rendering otherwise.
void draw_blocks_3d(void** ptr, int count) {
    for (int i = 0; i < count; i++) {
        if (ptr[i] == nullptr) continue;
        DrawData* data = (DrawData*)ptr[i];

        bool is_block = (data->vertex_count == 4 &&
                         data->vertices[0] == -0.05f &&
                         data->vertices[1] == -0.05f);

        if (is_block) {
            draw_block_3d(data);
        } else {
            glBegin(GL_TRIANGLE_FAN);
                glColor3f(data->r, data->g, data->b);
                for (int j = 0; j < data->vertex_count; j++) {
                    glVertex2f(data->vertices[j * 2] + data->x,
                              data->vertices[j * 2 + 1] + data->y);
                }
            glEnd();
        }
    }
}

static float full_size_block_offset = 0.1f;
static float mini_block_offset = 0.01f;

DrawData* new_obj_sized(StaticAllocator& sa, float X, float Y, float R, float G, float B, float size) {
    DrawData* sprite = new DrawData{};
    float half = size / 2.0f;
    sprite->vertex_count = 4;
    sprite->vertices[0] = -half; sprite->vertices[1] = -half;
    sprite->vertices[2] =  half; sprite->vertices[3] = -half;
    sprite->vertices[4] =  half; sprite->vertices[5] =  half;
    sprite->vertices[6] = -half; sprite->vertices[7] =  half;
    sprite->r = R;
    sprite->g = G;
    sprite->b = B;
    sprite->x = X;
    sprite->y = Y;
    sa.store_ptr(sprite);
    return sprite;
}

DrawData* new_obj(StaticAllocator& sa, float X, float Y, float R, float G, float B) {
    return new_obj_sized(sa, X, Y, R, G, B, full_size_block_offset);
}

std::array<DrawData*, 4> Larry(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;
    return {
        new_obj(sa, -h, base_y,          1.0f, 0.5f, 0.0f),  // orange
        new_obj(sa,  h, base_y,          1.0f, 0.5f, 0.0f),
        new_obj(sa,  h, base_y + o,      1.0f, 0.5f, 0.0f),
        new_obj(sa,  h, base_y + o * 2,  1.0f, 0.5f, 0.0f)
    };
}

std::array<DrawData*, 4> Terry(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;

    return {
        new_obj(sa,  h, base_y,          0.0f, 0.4f, 1.0f),  // blue
        new_obj(sa, -h, base_y,          0.0f, 0.4f, 1.0f),
        new_obj(sa, -h, base_y + o,      0.0f, 0.4f, 1.0f),
        new_obj(sa, -h, base_y + o * 2,  0.0f, 0.4f, 1.0f)
    };
}

std::array<DrawData*, 4> Berry(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;

    return {
        new_obj(sa,  h, base_y + o * 2,          1.0f, 0.4f, 1.0f),  // pink
        new_obj(sa, -h, base_y + o * 2,          1.0f, 0.4f, 1.0f),
        new_obj(sa, -h, base_y + o,      1.0f, 0.4f, 1.0f),
        new_obj(sa, -h, base_y,  1.0f, 0.4f, 1.0f)
    };
}

std::array<DrawData*, 4> Gerry(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;

    return {
        new_obj(sa,  -h, base_y + o * 2,          1.0f, 0.4f, 0.35f),  // reddish-orange?
        new_obj(sa, h, base_y + o * 2,          1.0f, 0.4f, 0.35f), 
        new_obj(sa, h, base_y + o,      1.0f, 0.4f, 0.35f),
        new_obj(sa, h, base_y,  1.0f, 0.4f, 0.35f)
    };
}


std::array<DrawData*, 4> Ivan(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.6f;
    return {
        new_obj(sa, -h, base_y,          0.0f, 0.9f, 0.9f),  // cyan
        new_obj(sa, -h, base_y + o,      0.0f, 0.9f, 0.9f),
        new_obj(sa, -h, base_y + o * 2,  0.0f, 0.9f, 0.9f),
        new_obj(sa, -h, base_y + o * 3,  0.0f, 0.9f, 0.9f)
    };
}

std::array<DrawData*, 4> Griffin(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;
    return {
        new_obj(sa, -h, base_y,      1.0f, 0.9f, 0.0f),  // yellow
        new_obj(sa,  h, base_y,      1.0f, 0.9f, 0.0f),
        new_obj(sa, -h, base_y + o,  1.0f, 0.9f, 0.0f),
        new_obj(sa,  h, base_y + o,  1.0f, 0.9f, 0.0f)
    };
}

std::array<DrawData*, 4> Zach(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;
    return {
        new_obj(sa, -h, base_y,      0.5f, 0.9f, 0.7f),  // white-green?
        new_obj(sa,  h, base_y,      0.5f, 0.9f, 0.7f),
        new_obj(sa, -h, base_y + o,  0.5f, 0.9f, 0.7f),
        new_obj(sa,  -h, base_y + o,  0.5f, 0.9f, 0.7f) // same positioning as previous to hide it
    };
}

// Short-L family -------------------------------------------------------
// These are the 4 rotations of the 2-tall / 1-to-the-side mini piece.
// Each uses a duplicate 4th block (same position as block 3) like Zach.

// Pip: top-right arm (horizontal mirror of Zach)
//   [X]
// [X][X]
std::array<DrawData*, 4> Pip(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;
    return {
        new_obj(sa, -h, base_y,         0.5f, 1.0f, 0.3f),  // lime
        new_obj(sa,  h, base_y,         0.5f, 1.0f, 0.3f),
        new_obj(sa,  h, base_y + o,     0.5f, 1.0f, 0.3f),
        new_obj(sa,  h, base_y + o,     0.5f, 1.0f, 0.3f)   // duplicate
    };
}

// Pop: arm bottom-right (vertical flip of Pip)
// [X][X]
//    [X]
std::array<DrawData*, 4> Pop(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;
    return {
        new_obj(sa, -h, base_y + o,    1.0f, 0.65f, 0.5f),  // coral
        new_obj(sa,  h, base_y + o,    1.0f, 0.65f, 0.5f),
        new_obj(sa,  h, base_y,        1.0f, 0.65f, 0.5f),
        new_obj(sa,  h, base_y,        1.0f, 0.65f, 0.5f)   // duplicate
    };
}

// Zup: arm bottom-left (horizontal mirror of Pop = vertical flip of Zach)
// [X][X]
// [X]
std::array<DrawData*, 4> Zup(StaticAllocator& sa) {
    float o = full_size_block_offset;
    float h = o / 2.0f;
    float base_y = 0.7f;
    return {
        new_obj(sa, -h, base_y + o,    0.6f, 0.6f, 1.0f),  // periwinkle
        new_obj(sa,  h, base_y + o,    0.6f, 0.6f, 1.0f),
        new_obj(sa, -h, base_y,        0.6f, 0.6f, 1.0f),
        new_obj(sa, -h, base_y,        0.6f, 0.6f, 1.0f)   // duplicate
    };
}

// Mini shapes (1/10th size)

std::array<DrawData*, 4> MiniLarry(StaticAllocator& sa, float base_x, float base_y) {
    float o = mini_block_offset;
    float h = o / 2.0f;
    return {
        new_obj_sized(sa, base_x - h, base_y,          1.0f, 0.5f, 0.0f, o),
        new_obj_sized(sa, base_x + h, base_y,          1.0f, 0.5f, 0.0f, o),
        new_obj_sized(sa, base_x + h, base_y + o,      1.0f, 0.5f, 0.0f, o),
        new_obj_sized(sa, base_x + h, base_y + o * 2,  1.0f, 0.5f, 0.0f, o)
    };
}

std::array<DrawData*, 4> MiniTerry(StaticAllocator& sa, float base_x, float base_y) {
    float o = mini_block_offset;
    float h = o / 2.0f;
    return {
        new_obj_sized(sa, base_x + h, base_y,          0.0f, 0.4f, 1.0f, o),
        new_obj_sized(sa, base_x - h, base_y,          0.0f, 0.4f, 1.0f, o),
        new_obj_sized(sa, base_x - h, base_y + o,      0.0f, 0.4f, 1.0f, o),
        new_obj_sized(sa, base_x - h, base_y + o * 2,  0.0f, 0.4f, 1.0f, o)
    };
}

std::array<DrawData*, 4> MiniIvan(StaticAllocator& sa, float base_x, float base_y) {
    float o = mini_block_offset;
    float h = o / 2.0f;
    return {
        new_obj_sized(sa, base_x - h, base_y,          0.0f, 0.9f, 0.9f, o),
        new_obj_sized(sa, base_x - h, base_y + o,      0.0f, 0.9f, 0.9f, o),
        new_obj_sized(sa, base_x - h, base_y + o * 2,  0.0f, 0.9f, 0.9f, o),
        new_obj_sized(sa, base_x - h, base_y + o * 3,  0.0f, 0.9f, 0.9f, o)
    };
}

std::array<DrawData*, 4> MiniGriffin(StaticAllocator& sa, float base_x, float base_y) {
    float o = mini_block_offset;
    float h = o / 2.0f;
    return {
        new_obj_sized(sa, base_x - h, base_y,      1.0f, 0.9f, 0.0f, o),
        new_obj_sized(sa, base_x + h, base_y,      1.0f, 0.9f, 0.0f, o),
        new_obj_sized(sa, base_x - h, base_y + o,  1.0f, 0.9f, 0.0f, o),
        new_obj_sized(sa, base_x + h, base_y + o,  1.0f, 0.9f, 0.0f, o)
    };
}