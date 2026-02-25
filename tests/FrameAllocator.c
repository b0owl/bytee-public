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


#include "../engine/include/allocator.h"
#include <iostream>

#define GREEN "\033[1;32m"
#define RED   "\033[1;31m"
#define RESET "\033[0m"

int main() {
    FrameAllocator fa;

    /* Test #1; does creating pointers work? */
    fa.create_pointers(1);
    if (fa.ptr != NULL) std::cout << GREEN "   Test 1 Passed!" RESET << std::endl;
    else std::cout << RED " 󰍹  Test 1 Failed!" RESET << std::endl;

    /* Test #2; does storing to pointers work? */
    DrawData* temp = new DrawData{ {0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 1.0f, 
                                     1.0f, 0.0f, 1.0f, 1.0f, 0.0f}, 6, 1.0f, 0.5f,
                                     0.0f, 0.0f, 0.0f, 1.0f, 1.0f };
    fa.store_ptr(temp);
    if (fa.ptr[0] != nullptr) std::cout << GREEN "   Test 2 Passed!" RESET << std::endl;
    else std::cout << RED " 󰍹  Test 2 Failed!" RESET << std::endl;

    /* Test #3; does end_frame clear pointers? */
    fa.end_frame();
    if (fa.ptr[0] == nullptr) std::cout << GREEN "   Test 3 Passed!" RESET << std::endl;
    else std::cout << RED " 󰍹  Test 3 Failed!" RESET << std::endl;

    return 0;
}
