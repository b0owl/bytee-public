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

#include "../include/allocator.h"
#include <cstdlib>

/* Manages persistent heap allocations. Stored pointers are NOT automatically
cleared - they live until the Allocator is destroyed or explicitly freed. */
Allocator::Allocator() {
    ptr = nullptr;
    m_pointers = 0;
    m_next_index = 0;
}

void Allocator::create_pointers(int size) {
    if (ptr != nullptr) {
        free(ptr);
    }

    m_pointers = size;
    ptr = (void**)malloc(m_pointers * sizeof(void*));

    for (int i = 0; i < m_pointers; i++) {
        ptr[i] = nullptr;
    }
}

void Allocator::draw_struct(void** ptr, int count) {
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

Allocator::~Allocator() {
    if (ptr != nullptr) {
        for (int i = 0; i < m_pointers; i++) {
            if (ptr[i] != nullptr) {
                delete static_cast<DrawData*>(ptr[i]);
            }
        }
        free(ptr);
    }
}
