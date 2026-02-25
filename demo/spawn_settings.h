#pragma once

constexpr int PIECE_COUNT = 10;

enum PieceIdx : int {
    PIECE_LARRY   = 0,
    PIECE_TERRY   = 1,
    PIECE_IVAN    = 2,
    PIECE_GRIFFIN = 3,
    PIECE_BERRY   = 4,
    PIECE_GERRY   = 5,
    PIECE_ZACH    = 6,
    PIECE_PIP     = 7,  // inverted Zach (top-right arm)
    PIECE_POP     = 8,  // Pip upside-down (top-row + bottom-right)
    PIECE_ZUP     = 9,  // Pop inverted (top-row + bottom-left)
};

// Piece families for type-weighted spawning.
// Each family has equal spawn probability; pieces within a family are equally likely.
// Family 0: I         {Ivan}
// Family 1: O         {Griffin}
// Family 2: S         {Berry}
// Family 3: Z         {Gerry}
// Family 4: L-long    {Larry, Terry}
// Family 5: L-short   {Zach, Pip, Pop, Zup}

struct SpawnSettings {
    bool enabled[PIECE_COUNT];

    SpawnSettings() {
        for (int i = 0; i < PIECE_COUNT; i++) enabled[i] = true;
    }

    static SpawnSettings& get() {
        static SpawnSettings s;
        return s;
    }
};
