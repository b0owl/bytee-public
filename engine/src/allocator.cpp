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


// TODO - Work on safer / cleaner allocator systems (Customizeable allocator object? // Would require refac)
// TODO - Add documentation

#include <cmath>
#include "../include/allocator.h"

// Memory tracking implementation
int MALLOCED_C = 0;

void* MALLOC(size_t size) {
    MALLOCED_C++;
    return malloc(size);
}

void FREE(void* ptr) {
    MALLOCED_C--;
    free(ptr);
}

/* Allows the engine to create and interact with pointers.
Pointers created will be automatically cleared and deleted every frame. 
WARNING: Creating new pointers will free the previous! It is up to you to prevent
unpredictable behavior! */
// Fair warning, this is a generally dangerous class to intereact with (obviously)
FrameAllocator::FrameAllocator() {
    ptr = NULL;
    m_pointers = 0;
    m_frames_passed = 0;
    m_next_index = 0;
}

void FrameAllocator::create_pointers(int size) {
    if (ptr != NULL) {
        FREE(ptr);
    }
    
    m_pointers = size;
    ptr = (void**)MALLOC(m_pointers * sizeof(void*));  
    
    for (int i = 0; i < m_pointers; i++) {
        ptr[i] = nullptr; // init pointers
    }
}

/* automatically frees pointers every frame */
void FrameAllocator::end_frame() {
    for (int i = 0; i < m_pointers; i++) {
        operator delete(ptr[i]);
        ptr[i] = nullptr;
    }
    m_next_index = 0;
}

void FrameAllocator::draw_struct(void** ptr, int count) {
    for (int i = 0; i < count; i++) {
        if (ptr[i] != nullptr) {
            DrawData* data = (DrawData*)ptr[i];
            
            glBegin(GL_TRIANGLE_FAN);
                glColor3f(data->r, data->g, data->b);
                for (int j = 0; j < data->vertex_count; j++) {
                    // Apply x,y offset to vertices
                    glVertex2f(data->vertices[j * 2] + data->x, 
                              data->vertices[j * 2 + 1] + data->y);
                }
            glEnd();
        }
    }
}

FrameAllocator::~FrameAllocator() {
    if (ptr != NULL) {
        end_frame();  
        FREE(ptr);    
    }
}

/* Manages persistent heap allocations that live until explicitly freed.
Unlike FrameAllocator, stored pointers are NOT automatically cleared */
StaticAllocator::StaticAllocator() {
    ptr = NULL;
    m_pointers = 0;
    m_next_index = 0;
}

void StaticAllocator::create_pointers(int size) {
    if (ptr != NULL) {
        FREE(ptr);
    }
    
    m_pointers = size;
    ptr = (void**)MALLOC(m_pointers * sizeof(void*));  
    
    for (int i = 0; i < m_pointers; i++) {
        ptr[i] = nullptr; 
    }
}

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

    // 1. Dark border outline (full block size)
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(r * 0.2f, g * 0.2f, b * 0.2f);
        glVertex2f(ox - H,      oy - H);
        glVertex2f(ox + H,      oy - H);
        glVertex2f(ox + H,      oy + H);
        glVertex2f(ox - H,      oy + H);
    glEnd();

    // 2. Highlight layer (top-left bevel) - inset by border
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(clampf(r * 1.4f, 0, 1), clampf(g * 1.4f, 0, 1), clampf(b * 1.4f, 0, 1));
        glVertex2f(ox - H + BD, oy - H + BD);
        glVertex2f(ox + H - BD, oy - H + BD);
        glVertex2f(ox + H - BD, oy + H - BD);
        glVertex2f(ox - H + BD, oy + H - BD);
    glEnd();

    // 3. Shadow layer (bottom-right bevel) - covers bottom and right strips
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(r * 0.45f, g * 0.45f, b * 0.45f);
        glVertex2f(ox - H + BD + BV, oy - H + BD);
        glVertex2f(ox + H - BD,      oy - H + BD);
        glVertex2f(ox + H - BD,      oy + H - BD - BV);
        glVertex2f(ox - H + BD + BV, oy + H - BD - BV);
    glEnd();

    // 4. Face (center) - the main block color
    glBegin(GL_TRIANGLE_FAN);
        glColor3f(r, g, b);
        glVertex2f(ox - H + BD + BV, oy - H + BD + BV);
        glVertex2f(ox + H - BD - BV, oy - H + BD + BV);
        glVertex2f(ox + H - BD - BV, oy + H - BD - BV);
        glVertex2f(ox - H + BD + BV, oy + H - BD - BV);
    glEnd();
}

void StaticAllocator::draw_struct(void** ptr, int count) {
    for (int i = 0; i < count; i++) {
        if (ptr[i] != nullptr) {
            DrawData* data = (DrawData*)ptr[i];

            // Detect standard tetris blocks (0.1x0.1 squares from new_obj)
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
}

StaticAllocator::~StaticAllocator() {
    if (ptr != NULL) {
        for (int i = 0; i < m_pointers; i++) {
            if (ptr[i] != nullptr) {
                delete static_cast<DrawData*>(ptr[i]);  
            }
        }
        FREE(ptr);
    }
}