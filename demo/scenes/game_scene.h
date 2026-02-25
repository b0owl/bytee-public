#pragma once

#include "../../engine/src/scene_manager/scene_manager.h"
#include "../../engine/include/bytee.h"
#include "../objects.h"
#include <vector>
#include <array>

class GameScene : public Scene {
public:
    GameScene();
    SceneID tick(GLFWwindow* window, float delta_time,
                 float cur_aspect, int fb_w, int fb_h) override;

private:
    static constexpr float BLOCK_SIZE          = 0.1f;
    static constexpr float COLLISION_THRESHOLD = 0.08f;
    static constexpr float PLAY_X              = 0.9f;
    static constexpr float BOUND_EPS           = 0.001f;
    static constexpr int   NUM_COLS            = 18;
    static constexpr float DAS_DELAY           = 0.2f;
    static constexpr float DAS_RATE            = 0.05f;

    StaticAllocator sa;
    std::vector<DrawData*> g_sprite;
    std::vector<DrawData*> g_active_piece;
    std::vector<DrawData*> g_locked_pieces;
    std::array<DrawData*, 4> walls;

    float piece_fall_timer  = 0.5f;
    float timer             = 0.0f;
    bool  stop_piece_gravity = false;

    bool  prev_left        = false;
    bool  prev_right       = false;
    float left_held_time   = 0.0f;
    float right_held_time  = 0.0f;

    bool check_collision_between(DrawData* a, DrawData* b);
    std::array<DrawData*, 4> spawn_random_shape();
    void clear_full_rows();
    void reset_game();
    std::array<DrawData*, 4> create_boundaries();
};
