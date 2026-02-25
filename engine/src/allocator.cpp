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

// Note: This allocator at some point in time was re-written by claude code. I didn't tell it to edit this file
// but for some reason it decided it was the best solution to a problem I asked it to solve. I stupidly didn't review this code,
// nor did I notice the changes. As of the time im writing this, im re-doing the allocator system, but I want it to be clear for 
// anyone browsing early versions of this project, the code generally sucking is the fault of claude.

// Slightly later note: Actually realized there isn't much to rewrite. Someone recommended using smart pointers,
// which I may pursue, but for the time being, simply removing the tetris-specific logic works.

// Details:
//      It added tetris (demo) specific code to this file. 
//      It created a new function called draw_block_3d, not relevant to the allocator itself.
//      It also created the memory tracking implementation? No clue why.

#include "../include/allocator.h"
#include <cstdlib>

/* Allows the engine to create and interact with pointers.
Pointers created will be automatically cleared and deleted every frame. 
WARNING: Creating new pointers will free the previous! It is up to you to prevent
unpredictable behavior! */
FrameAllocator::FrameAllocator() {
    ptr = nullptr;
    m_pointers = 0;
    m_frames_passed = 0;
    m_next_index = 0;
}

void FrameAllocator::create_pointers(int size) {
    if (ptr != nullptr) {
        free(ptr);
    }
    
    m_pointers = size;
    ptr = (void**)malloc(m_pointers * sizeof(void*));  
    
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
    if (ptr != nullptr) {
        end_frame();  
        free(ptr);    
    }
}

/* Manages persistent heap allocations that live until explicitly freed.
Unlike FrameAllocator, stored pointers are NOT automatically cleared */
StaticAllocator::StaticAllocator() {
    ptr = nullptr;
    m_pointers = 0;
    m_next_index = 0;
}

void StaticAllocator::create_pointers(int size) {
    if (ptr != nullptr) {
        free(ptr);
    }
    
    m_pointers = size;
    ptr = (void**)malloc(m_pointers * sizeof(void*));  
    
    for (int i = 0; i < m_pointers; i++) {
        ptr[i] = nullptr; 
    }
}

void StaticAllocator::draw_struct(void** ptr, int count) {
    for (int i = 0; i < count; i++) {
        if (ptr[i] != nullptr) {
            DrawData* data = (DrawData*)ptr[i];
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

StaticAllocator::~StaticAllocator() {
    if (ptr != nullptr) {
        for (int i = 0; i < m_pointers; i++) {
            if (ptr[i] != nullptr) {
                delete static_cast<DrawData*>(ptr[i]);  
            }
        }
        free(ptr);
    }
}