// TODO - Add rotation logic instead of multiple rotated versions of the same piece [ ]
// TODO - Scoring system [ ]

// General TODO - Add a proper debugging system

#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

#include <cmath>
#include <algorithm>
#include <random>
#include <vector>

#include <iostream> // might be unused at time of commit, added for debug print purposes

#include "game_scene.h"
#include "../spawn_settings.h"

static int score = 100; 

GameScene::GameScene() {
    sa.create_pointers(256);
    walls = create_boundaries();
    for (auto& wall : walls) g_sprite.push_back(wall);

    auto shapes = spawn_random_shape();
    g_sprite.insert(g_sprite.end(), shapes.begin(), shapes.end());
    g_active_piece.assign(shapes.begin(), shapes.end());
}

bool GameScene::check_collision_between(DrawData* a, DrawData* b) {
    float x_diff = std::abs(a->x - b->x);
    float y_diff = std::abs(a->y - b->y);
    return (x_diff < COLLISION_THRESHOLD && y_diff < COLLISION_THRESHOLD);
}

std::array<DrawData*, 4> GameScene::spawn_random_shape() {
    using PieceFn = std::array<DrawData*, 4>(*)(StaticAllocator&);

    // Indexed to match PieceIdx enum in spawn_settings.h
    static const PieceFn all_pieces[PIECE_COUNT] = {
        Larry, Terry, Ivan, Griffin, Berry, Gerry, Zach, Pip, Pop, Zup
    };

    // Families: each family has equal spawn weight;
    // pieces within a family share that weight equally.
    static const std::vector<std::vector<int>> families = {
        { PIECE_IVAN    },                                      // I
        { PIECE_GRIFFIN },                                      // O
        { PIECE_BERRY   },                                      // S
        { PIECE_GERRY   },                                      // Z
        { PIECE_LARRY,  PIECE_TERRY                        },   // L-long
        { PIECE_ZACH,   PIECE_PIP, PIECE_POP, PIECE_ZUP   },   // L-short
    };

    static std::random_device rd;
    static std::mt19937 gen(rd());

    const SpawnSettings& ss = SpawnSettings::get();

    // Collect enabled families (any piece in the family is enabled)
    std::vector<std::vector<int>> enabled_families;
    for (const auto& fam : families) {
        std::vector<int> enabled_in;
        for (int idx : fam)
            if (ss.enabled[idx]) enabled_in.push_back(idx);
        if (!enabled_in.empty()) enabled_families.push_back(enabled_in);
    }

    if (enabled_families.empty()) return Larry(sa);  // fallback: nothing enabled

    std::uniform_int_distribution<> fam_dis(0, (int)enabled_families.size() - 1);
    const auto& chosen_fam = enabled_families[fam_dis(gen)];

    std::uniform_int_distribution<> piece_dis(0, (int)chosen_fam.size() - 1);
    return all_pieces[chosen_fam[piece_dis(gen)]](sa);
}

void GameScene::clear_full_rows() {
    std::vector<float> row_ys;
    for (auto* block : g_locked_pieces) {
        bool found = false;
        for (float ry : row_ys) {
            if (std::abs(block->y - ry) < BLOCK_SIZE / 2) { found = true; break; }
        }
        if (!found) row_ys.push_back(block->y);
    }

    std::vector<float> full_ys;
    for (float ry : row_ys) {
        int count = 0;
        for (auto* block : g_locked_pieces) {
            if (std::abs(block->y - ry) < BLOCK_SIZE / 2) count++;
        }
        if (count >= NUM_COLS) full_ys.push_back(ry);
    }
    if (full_ys.empty()) return;

    score += 100 * full_ys.size();

    std::sort(full_ys.begin(), full_ys.end());

    std::vector<DrawData*> to_remove;
    for (auto* block : g_locked_pieces) {
        for (float fy : full_ys) {
            if (std::abs(block->y - fy) < BLOCK_SIZE / 2) {
                to_remove.push_back(block);
                break;
            }
        }
    }

    for (auto* ptr : to_remove) {
        g_locked_pieces.erase(
            std::remove(g_locked_pieces.begin(), g_locked_pieces.end(), ptr),
            g_locked_pieces.end());
        g_sprite.erase(
            std::remove(g_sprite.begin(), g_sprite.end(), ptr),
            g_sprite.end());
    }

    for (int i = 0; i < (int)full_ys.size(); i++) {
        for (auto* block : g_locked_pieces) {
            if (block->y > full_ys[i] + BLOCK_SIZE / 2)
                block->y -= BLOCK_SIZE;
        }
        for (int j = i + 1; j < (int)full_ys.size(); j++)
            full_ys[j] -= BLOCK_SIZE;
    }
}

void GameScene::reset_game() {
    g_sprite.clear();
    g_active_piece.clear();
    g_locked_pieces.clear();
    sa.reset_index();

    for (auto& wall : walls) g_sprite.push_back(wall);

    auto shapes = spawn_random_shape();
    g_sprite.insert(g_sprite.end(), shapes.begin(), shapes.end());
    g_active_piece.assign(shapes.begin(), shapes.end());
}

std::array<DrawData*, 4> GameScene::create_boundaries() {
    const float WT = 0.005f;
    const float WH = WT / 2.0f;
    const float PY = 0.95f;

    float lr_wall[8] = {
        -WH, -(PY + WT),
         WH, -(PY + WT),
         WH,  (PY + WT),
        -WH,  (PY + WT)
    };
    float tb_wall[8] = {
        -PLAY_X, -WH,
         PLAY_X, -WH,
         PLAY_X,  WH,
        -PLAY_X,  WH
    };

    return {
        createobj(lr_wall, 4, -(PLAY_X + WH), 0.0f,       0.3f, 0.2f, 0.1f),
        createobj(lr_wall, 4,  (PLAY_X + WH), 0.0f,       0.3f, 0.2f, 0.1f),
        createobj(tb_wall, 4,  0.0f,          -(PY + WH), 0.3f, 0.2f, 0.1f),
        createobj(tb_wall, 4,  0.0f,           (PY + WH), 0.3f, 0.2f, 0.1f)
    };
}

SceneID GameScene::tick(GLFWwindow* window, float delta_time,
                         float cur_aspect, int fb_w, int fb_h) {
    // ESC returns to title screen
    if (is_key_down(window, GLFW_KEY_ESCAPE))
        return SceneID::Title;

    // Speed
    if (is_key_down(window, GLFW_KEY_S) || is_key_down(window, GLFW_KEY_DOWN)) {
        piece_fall_timer = 0.1f;
    } else {
        piece_fall_timer = 0.5f;
    }

    // DAS (Delayed Auto Shift) movement
    {
        float move_amount = BLOCK_SIZE;
        bool cur_left  = is_key_down(window, GLFW_KEY_LEFT) || is_key_down(window, GLFW_KEY_A);
        bool cur_right = is_key_down(window, GLFW_KEY_RIGHT) || is_key_down(window, GLFW_KEY_D);

        bool move_left = false, move_right = false;

        if (cur_left) {
            if (!prev_left) { move_left = true; left_held_time = 0.0f; }
            else {
                left_held_time += delta_time;
                if (left_held_time >= DAS_DELAY) { left_held_time -= DAS_RATE; move_left = true; }
            }
        } else { left_held_time = 0.0f; }

        if (cur_right) {
            if (!prev_right) { move_right = true; right_held_time = 0.0f; }
            else {
                right_held_time += delta_time;
                if (right_held_time >= DAS_DELAY) { right_held_time -= DAS_RATE; move_right = true; }
            }
        } else { right_held_time = 0.0f; }

        prev_left  = cur_left;
        prev_right = cur_right;

        if (move_left) {
            bool can_move = true;
            for (auto& sprite : g_active_piece) {
                if (sprite->x - move_amount < -PLAY_X + BLOCK_SIZE / 2 - BOUND_EPS) {
                    can_move = false; break;
                }
                for (auto& locked : g_locked_pieces) {
                    DrawData temp = *sprite;
                    temp.x -= move_amount;
                    if (check_collision_between(&temp, locked)) { can_move = false; break; }
                }
                if (!can_move) break;
            }
            if (can_move)
                for (auto& sprite : g_active_piece) sprite->x -= move_amount;
        }

        if (move_right) {
            bool can_move = true;
            for (auto& sprite : g_active_piece) {
                if (sprite->x + move_amount > PLAY_X - BLOCK_SIZE / 2 + BOUND_EPS) {
                    can_move = false; break;
                }
                for (auto& locked : g_locked_pieces) {
                    DrawData temp = *sprite;
                    temp.x += move_amount;
                    if (check_collision_between(&temp, locked)) { can_move = false; break; }
                }
                if (!can_move) break;
            }
            if (can_move)
                for (auto& sprite : g_active_piece) sprite->x += move_amount;
        }
    }

    // Gravity
    timer += delta_time;
    if (timer >= piece_fall_timer) {
        bool piece_landed = false;

        for (auto& sprite : g_active_piece) {
            if (!stop_piece_gravity &&
                sprite->y - BLOCK_SIZE <= -1.0f + BOUND_EPS) {
                piece_landed = true; break;
            }
        }

        if (!piece_landed && !g_locked_pieces.empty()) {
            for (auto& active_sprite : g_active_piece) {
                for (auto& locked_sprite : g_locked_pieces) {
                    DrawData temp = *active_sprite;
                    temp.y -= BLOCK_SIZE;
                    float x_diff = std::abs(temp.x - locked_sprite->x);
                    float y_diff = temp.y - locked_sprite->y;
                    if (x_diff < COLLISION_THRESHOLD &&
                        y_diff <= 0.02f && y_diff >= -BLOCK_SIZE) {
                        piece_landed = true; break;
                    }
                }
                if (piece_landed) break;
            }
        }

        if (piece_landed) stop_piece_gravity = true;
        else for (auto& sprite : g_active_piece) sprite->y -= BLOCK_SIZE;
        timer = 0.0f;
    }

    // Piece landing: lock and spawn next
    if (stop_piece_gravity) {
        g_locked_pieces.insert(g_locked_pieces.end(),
                               g_active_piece.begin(), g_active_piece.end());
        clear_full_rows();

        bool game_over = false;
        for (auto& locked : g_locked_pieces) {
            if (locked->y >= 1.0f - BLOCK_SIZE) { game_over = true; break; }
        }

        if (game_over) {
            reset_game();
        } else {
            auto new_shapes = spawn_random_shape();
            g_sprite.insert(g_sprite.end(), new_shapes.begin(), new_shapes.end());
            g_active_piece.assign(new_shapes.begin(), new_shapes.end());
        }
        stop_piece_gravity = false;
    }

    // Render
    float font_size = 0.02f;
    float corrected_w;
    unsigned int tex = draw_image("demo/settings_icon.png",
                                  cur_aspect - 0.17f, 0.83f, 0.15f, 0.15f, &corrected_w);
    define_widget_area(tex,
                       cur_aspect - 0.17f, cur_aspect - 0.17f + corrected_w,
                       0.83f, 0.83f + 0.15f);

    draw_blocks_3d((void**)g_sprite.data(), g_sprite.size());

    WidgetInfoArea wia = optimal_widget_info_area(tex, fb_w, fb_h, font_size,
                                                  "demo/font.ttf", "Settings");
    bool settings_hovered = check_widget_hover(window, tex, fb_w, fb_h, cur_aspect);
    if (settings_hovered)
        draw_text("demo/font.ttf", "Settings", wia.x - 0.02f, wia.y,
                  font_size, 1.0f, 1.0f, 1.0f);
    if (check_widget_click(window, tex, fb_w, fb_h, cur_aspect))
        return SceneID::Settings;

    // ESC hint in corner
    draw_text("demo/font.ttf", "ESC: menu",
              -cur_aspect + 0.03f, 0.95f, font_size, 0.5f, 0.5f, 0.5f);

    // Score in corner
    // Lazily multiplied the font size, mb
    draw_text("demo/font.ttf", std::to_string(score).c_str(),
            -cur_aspect + 0.02f, 0.85f, font_size * 3, 1.0f, 1.0f, 1.0f);

    return SceneID::None;
}
