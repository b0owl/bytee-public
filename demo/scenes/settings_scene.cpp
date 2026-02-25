#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

#include <cstdio>
#include "settings_scene.h"
#include "../spawn_settings.h"

static const struct { const char* name; int idx; } PIECES[] = {
    { "Larry",   PIECE_LARRY   },
    { "Terry",   PIECE_TERRY   },
    { "Ivan",    PIECE_IVAN    },
    { "Griffin", PIECE_GRIFFIN },
    { "Berry",   PIECE_BERRY   },
    { "Gerry",   PIECE_GERRY   },
    { "Zach",    PIECE_ZACH    },
    { "Pip",     PIECE_PIP     },
    { "Pop",     PIECE_POP     },
    { "Zup",     PIECE_ZUP     },
};

SceneID SettingsScene::tick(GLFWwindow* window, float delta_time,
                             float cur_aspect, int fb_w, int fb_h) {
    const char* font = "demo/font.ttf";
    constexpr float SZ    = 0.042f;
    constexpr float HDR   = 0.055f;

    // Edge-triggered click detection
    MouseState ms;
    get_mouse_state(window, ms);
    bool just_clicked    = ms.left_button && !prev_mouse_down;
    prev_mouse_down      = ms.left_button;

    // Header
    draw_text(font, "PIECE SETTINGS",
              -get_text_width(font, "PIECE SETTINGS", HDR) / 2.0f, 0.72f,
              HDR, 1.0f, 1.0f, 1.0f);

    // 2-column grid: left col = pieces 0-4, right col = pieces 5-9
    // 5 rows, starting at y=0.48 stepping down by 0.13
    constexpr float COL_L    = -0.50f;
    constexpr float COL_R    =  0.08f;
    constexpr float ROW_TOP  =  0.48f;
    constexpr float ROW_STEP = -0.13f;

    SpawnSettings& ss = SpawnSettings::get();

    for (int row = 0; row < 5; row++) {
        float y = ROW_TOP + row * ROW_STEP;

        for (int col = 0; col < 2; col++) {
            int i    = row + col * 5;
            float x  = col == 0 ? COL_L : COL_R;
            bool en  = ss.enabled[PIECES[i].idx];

            char label[32];
            std::snprintf(label, sizeof(label), "%s %s",
                          en ? "[X]" : "[ ]", PIECES[i].name);

            float r = en ? 1.0f : 0.4f;
            float g = en ? 1.0f : 0.4f;
            float b = en ? 1.0f : 0.4f;

            draw_text(font, label, x, y, SZ, r, g, b);
            define_text_area(PIECES[i].name, font, label, x, y, SZ);

            if (just_clicked &&
                check_text_hover(window, PIECES[i].name, fb_w, fb_h, cur_aspect)) {
                ss.enabled[PIECES[i].idx] ^= true;
            }
        }
    }

    // BACK button
    float back_x = -get_text_width(font, "BACK", SZ) / 2.0f;
    draw_text(font, "BACK", back_x, -0.65f, SZ, 1.0f, 1.0f, 1.0f);
    define_text_area("back_btn", font, "BACK", back_x, -0.65f, SZ);

    if (just_clicked &&
        check_text_hover(window, "back_btn", fb_w, fb_h, cur_aspect))
        return SceneID::Title;

    if (is_key_down(window, GLFW_KEY_ESCAPE))
        return SceneID::Title;

    return SceneID::None;
}
