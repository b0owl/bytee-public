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


#ifndef OBJECTS_H
#define OBJECTS_H

#include <array>
#include "../engine/include/allocator.h"
#include "../engine/include/rendering.h"

DrawData* new_obj_sized(StaticAllocator& sa, float X, float Y, float R, float G, float B, float size);
DrawData* new_obj(StaticAllocator& sa, float X, float Y, float R, float G, float B);

void draw_blocks_3d(void** ptr, int count);

std::array<DrawData*, 4> Larry(StaticAllocator& sa);
std::array<DrawData*, 4> Terry(StaticAllocator& sa);
std::array<DrawData*, 4> Ivan(StaticAllocator& sa);
std::array<DrawData*, 4> Griffin(StaticAllocator& sa);
std::array<DrawData*, 4> Berry(StaticAllocator& sa);
std::array<DrawData*, 4> Gerry(StaticAllocator& sa);
std::array<DrawData*, 4> Zach(StaticAllocator& sa);

// Short-L family (2-tall, 1 to the side — rotations/inversions of Zach)
std::array<DrawData*, 4> Pip(StaticAllocator& sa);  // inverted Zach   ([X][X] / [X])
std::array<DrawData*, 4> Pop(StaticAllocator& sa);  // Pip upside-down ([X] / [X][X])
std::array<DrawData*, 4> Zup(StaticAllocator& sa);  // Pop inverted    ([X] \ [X][X])

std::array<DrawData*, 4> MiniLarry(StaticAllocator& sa, float base_x, float base_y);
std::array<DrawData*, 4> MiniTerry(StaticAllocator& sa, float base_x, float base_y);
std::array<DrawData*, 4> MiniIvan(StaticAllocator& sa, float base_x, float base_y);
std::array<DrawData*, 4> MiniGriffin(StaticAllocator& sa, float base_x, float base_y);

#endif