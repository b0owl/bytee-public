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


#ifndef COLLISIONS_H
#define COLLISIONS_H
#include "allocator.h"
#include <vector>

template<typename AllocatorType>
std::vector<int> return_dims(AllocatorType& allocator, std::vector<DrawData>& entidvec) { 
    entidvec.reserve(allocator.m_pointers);
    std::vector<int> dims;
    dims.reserve(allocator.m_pointers * 2);
    
    for (int i = 0; i < allocator.m_pointers; i++) {
        entidvec.push_back(*((DrawData*)allocator.ptr[i]));
    }
    
    for (int i = 0; i < entidvec.size(); i++) {
        dims.push_back(entidvec[i].width); 
        dims.push_back(entidvec[i].height);
    }
    
    return dims;
}

template<typename AllocatorType>
std::vector<int> return_coords(AllocatorType& allocator, std::vector<DrawData>& entidvec) { 
    entidvec.reserve(allocator.m_pointers);
    std::vector<int> coords;
    coords.reserve(allocator.m_pointers * 2);  
    
    for (int i = 0; i < allocator.m_pointers; i++) {
        entidvec.push_back(*((DrawData*)allocator.ptr[i]));
    }
    
    for (int i = 0; i < entidvec.size(); i++) {
        coords.push_back(entidvec[i].x); 
        coords.push_back(entidvec[i].y);
    }
    
    return coords;
}

template<typename AllocatorType>
int return_dist(AllocatorType& allocator, std::vector<DrawData>& entidvec1, std::vector<DrawData>& entidvec2) {
    std::vector<int> coords1 = return_coords(allocator, entidvec1);
    std::vector<int> coords2 = return_coords(allocator, entidvec2);
    
    int x_diff = coords1[0] - coords2[0]; 
    int y_diff = coords1[1] - coords2[1];
    return x_diff + y_diff;  
}

template<typename AllocatorType>
bool is_colliding(AllocatorType& allocator, std::vector<DrawData>& entidvec1, std::vector<DrawData>& entidvec2) {
    std::vector<int> coords1 = return_coords(allocator, entidvec1);
    std::vector<int> coords2 = return_coords(allocator, entidvec2);
    std::vector<int> dims = return_dims(allocator, entidvec2);
    
    int x_diff = coords1[0] - coords2[0]; 
    int y_diff = coords1[1] - coords2[1];
    
    if (x_diff <= dims[0] && y_diff <= dims[1]) {
        return true;        
    }   
    return false;
}

#endif