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


#include "../engine/include/collisions.h"
#include <iostream>
#include <vector>

#define GREEN "\033[1;32m"
#define RED   "\033[1;31m"
#define RESET "\033[0m"

int main() {

    FrameAllocator fa;

	fa.create_pointers(2);
    
	DrawData* sprite1 = new DrawData();
	sprite1->vertices[5] = 1;
	sprite1->vertex_count = 4;
	sprite1->r = 1.0f;
	sprite1->g = 0.5f;
	sprite1->b = 0.0f;
	sprite1->x = 100.0f;
	sprite1->y = 200.0f;
	sprite1->width = 64.0f;
	sprite1->height = 32.0f;
	fa.store_ptr(sprite1);

	DrawData* sprite2 = new DrawData();
	sprite2->vertices[5] = 1;
	sprite2->vertex_count = 4;
	sprite2->r = 1.0f;
	sprite2->g = 0.5f;
	sprite2->b = 0.0f;
	sprite2->x = 100.0f;
	sprite2->y = 200.0f;
	sprite2->width = 64.0f;
	sprite2->height = 32.0f;
	fa.store_ptr(sprite2);

    std::vector<DrawData> obj1;
    std::vector<DrawData> obj2;


    obj1.push_back(*sprite1);
	obj2.push_back(*sprite2);

    if (return_dist(fa, obj1, obj2) == 0) std::cout << GREEN "  Test 1 Passed!" RESET << std::endl;
    else std::cout << RED "   Test 1 Failed!" RESET << std::endl;
    
    if (is_colliding(fa, obj1, obj2) == true) std::cout << GREEN "  Test 2 Passed!" RESET << std::endl;
	else std::cout << RED "   Test 2 Failed!" RESET << std::endl;

    return 0;
}
 
