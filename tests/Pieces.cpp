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

#include "demo/objects.h"
#include "demo/spawn_settings.h"
#include "engine/include/allocator.h"
#include <iostream>

#define GREEN "\033[1;32m"
#define RED   "\033[1;31m"
#define RESET "\033[0m"

static void test(bool ok, const char* name) {
    if (ok) std::cout << GREEN "   " << name << " Passed!" RESET << std::endl;
    else    std::cout << RED   " \xe2\x80\x8b  " << name << " Failed!" RESET << std::endl;
}

// Returns true if all 4 pointers in the piece are non-null
static bool all_non_null(const std::array<DrawData*, 4>& p) {
    for (auto* ptr : p) if (!ptr) return false;
    return true;
}

int main() {
    StaticAllocator sa;
    sa.create_pointers(128);

    // ── existing pieces return 4 non-null blocks ──────────────────────────
    sa.reset_index(); test(all_non_null(Larry(sa)),   "Larry returns 4 blocks");
    sa.reset_index(); test(all_non_null(Terry(sa)),   "Terry returns 4 blocks");
    sa.reset_index(); test(all_non_null(Ivan(sa)),    "Ivan returns 4 blocks");
    sa.reset_index(); test(all_non_null(Griffin(sa)), "Griffin returns 4 blocks");
    sa.reset_index(); test(all_non_null(Berry(sa)),   "Berry returns 4 blocks");
    sa.reset_index(); test(all_non_null(Gerry(sa)),   "Gerry returns 4 blocks");
    sa.reset_index(); test(all_non_null(Zach(sa)),    "Zach returns 4 blocks");

    // ── new pieces return 4 non-null blocks ───────────────────────────────
    sa.reset_index(); test(all_non_null(Pip(sa)),     "Pip returns 4 blocks");
    sa.reset_index(); test(all_non_null(Pop(sa)),     "Pop returns 4 blocks");
    sa.reset_index(); test(all_non_null(Zup(sa)),     "Zup returns 4 blocks");

    // ── new pieces have distinct arm positions from Zach ──────────────────
    // Zach: arm block at (-h, base_y+o)   => blocks[2].x < 0
    // Pip:  arm block at (+h, base_y+o)   => blocks[2].x > 0
    sa.reset_index(); auto zach = Zach(sa);
    sa.reset_index(); auto pip  = Pip(sa);
    test(zach[2]->x < 0 && pip[2]->x > 0, "Pip arm is opposite side to Zach arm");

    // Pop: top row (blocks 0,1 at base_y+o), single block at base_y
    sa.reset_index(); auto pop = Pop(sa);
    test(pop[0]->y > pop[2]->y, "Pop has top row above single bottom block");

    // Zup: top row (blocks 0,1 at base_y+o), single block at base_y on LEFT
    sa.reset_index(); auto zup = Zup(sa);
    test(zup[0]->y > zup[2]->y && zup[2]->x < 0,
         "Zup has top row above bottom-left arm");

    // Pop and Zup are mirror images (same y-layout, opposite bottom-block x)
    sa.reset_index(); auto pop2 = Pop(sa);
    sa.reset_index(); auto zup2 = Zup(sa);
    test(pop2[2]->x > 0 && zup2[2]->x < 0,
         "Pop and Zup bottom arms are on opposite sides");

    // ── SpawnSettings defaults all enabled ────────────────────────────────
    SpawnSettings& ss = SpawnSettings::get();
    bool all_on = true;
    for (int i = 0; i < PIECE_COUNT; i++) if (!ss.enabled[i]) { all_on = false; break; }
    test(all_on, "SpawnSettings defaults all pieces enabled");

    // ── toggling a piece persists ─────────────────────────────────────────
    ss.enabled[PIECE_PIP] = false;
    test(!ss.enabled[PIECE_PIP], "SpawnSettings toggle persists");
    ss.enabled[PIECE_PIP] = true;  // restore

    return 0;
}
