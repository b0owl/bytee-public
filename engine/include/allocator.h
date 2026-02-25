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


#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <GLFW/glfw3.h>
#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

// Memory tracking
extern int MALLOCED_C;
void* MALLOC(size_t size);
void FREE(void* ptr);

// Draw data structure
struct DrawData {
    float vertices[12];
    int vertex_count;
    float r, g, b;
    float x, y;
    float width, height;
};

// Frame-based memory allocator
class FrameAllocator {
public:
    void **ptr;
    int m_pointers;

private:
    int m_frames_passed;
    int m_next_index;

public:
    FrameAllocator();
    void create_pointers(int size);

    template<typename T>
    void store_ptr(T* value) {
        ptr[m_next_index] = value;
        m_next_index++;
        m_pointers = m_next_index;
    }

    void reset_index() { m_next_index = 0; }
    
    void end_frame();
    void draw_struct(void** ptr, int count);
    ~FrameAllocator();
};

// Static memory allocator
class StaticAllocator {
public:
    void **ptr;
    int m_pointers;

private:
    int m_next_index;

public:
    StaticAllocator();
    void create_pointers(int size);

    template<typename T>
    void store_ptr(T* value) {
        if (m_next_index >= 0 && m_next_index < m_pointers) {
            ptr[m_next_index] = value;
            m_next_index++;
        }
    }

    void reset_index() { m_next_index = 0; }
    
    void draw_struct(void** ptr, int count);
    ~StaticAllocator();
};

#endif
