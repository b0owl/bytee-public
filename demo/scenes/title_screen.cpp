#ifdef __APPLE__
  #include <OpenGL/gl.h>
#else
  #include <GL/gl.h>
#endif

#include <cmath>
#include "title_screen.h"
#include <string>

static std::string play_text = "PLAY";
static std::string settings_text = "SETTINGS";

float play_rgb[3] = {1.0, 1.0, 1.0};
float settings_rgb[3] = {1.0, 1.0, 1.0};

TitleScreen::TitleScreen() {
    sa.create_pointers(128);

    // Scatter decorative mini pieces around the screen edges
    auto add = [&](auto arr) {
        for (auto* p : arr) decorations.push_back(p);
    };

    add(MiniLarry(sa,  -0.80f, -0.65f));
    add(MiniTerry(sa,  -0.55f, -0.50f));
    add(MiniIvan(sa, -0.70f, -0.20f));
    add(MiniGriffin(sa,-0.40f, -0.80f));

    add(MiniLarry(sa,   0.60f, -0.60f));
    add(MiniTerry(sa,   0.75f, -0.30f));
    add(MiniIvan(sa,  0.40f, -0.75f));
    add(MiniGriffin(sa, 0.55f, -0.45f));
}

SceneID TitleScreen::tick(GLFWwindow* window, float delta_time,
                           float cur_aspect, int fb_w, int fb_h) {
    anim_time += delta_time;

    const char* font = "demo/font.ttf";

    sa.draw_struct((void**)decorations.data(), decorations.size());

    // Title + subtitle
    draw_text(font, "BYTEE DEMO", -get_text_width(font, "BYTEE DEMO", 0.14f) / 2.0f, 0.42f, 0.14f, 1.0f, 1.0f, 1.0f);
    draw_text(font, "tetris",     -get_text_width(font, "tetris",     0.055f) / 2.0f, 0.22f, 0.055f, 0.72f, 0.62f, 1.0f);

    // Divider line
    float line_hw = 0.30f, line_y = 0.14f;
    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_QUADS);
        glVertex2f(-line_hw, line_y - 0.003f);
        glVertex2f( line_hw, line_y - 0.003f);
        glVertex2f( line_hw, line_y + 0.003f);
        glVertex2f(-line_hw, line_y + 0.003f);
    glEnd();

    // Up/down arrow navigation
    bool up_now   = is_key_down(window, GLFW_KEY_UP);
    bool down_now = is_key_down(window, GLFW_KEY_DOWN);
    if (up_now   && !up_was_down)   selected = (selected + 1) % 2;
    if (down_now && !down_was_down) selected = (selected + 1) % 2;
    up_was_down   = up_now;
    down_was_down = down_now;

    // Mouse hover overrides keyboard selection
    bool play_hovered     = check_text_hover(window, "PLAY",     fb_w, fb_h, cur_aspect);
    bool settings_hovered = check_text_hover(window, "SETTINGS", fb_w, fb_h, cur_aspect);
    if (play_hovered)     selected = 0;
    if (settings_hovered) selected = 1;

    bool play_active     = (selected == 0);
    bool settings_active = (selected == 1);

    // PLAY button
    if (play_active) {
        play_text = "[ PLAY ]";
        for (int i = 0; i < 3; i++) play_rgb[i] = 0.5;
    } else {
        play_text = "PLAY";
        for (int i = 0; i < 3; i++) play_rgb[i] = 1.0;
    }
    float play_x = -get_text_width(font, play_text.c_str(), 0.038f) / 2.0f, play_y = -0.035f;
    draw_text(font, play_text.c_str(), play_x, play_y, 0.038f, play_rgb[0], play_rgb[1], play_rgb[2]);
    define_text_area("PLAY", font, play_text.c_str(), play_x, play_y, 0.038f);

    // SETTINGS button
    if (settings_active) {
        settings_text = "[ SETTINGS ]";
        for (int i = 0; i < 3; i++) settings_rgb[i] = 0.5;
    } else {
        settings_text = "SETTINGS";
        for (int i = 0; i < 3; i++) settings_rgb[i] = 1.0;
    }
    float set_x = -get_text_width(font, settings_text.c_str(), 0.034f) / 2.0f, set_y = -0.13f;
    draw_text(font, settings_text.c_str(), set_x, set_y, 0.034f, settings_rgb[0], settings_rgb[1], settings_rgb[2]);
    define_text_area("SETTINGS", font, settings_text.c_str(), set_x, set_y, 0.034f);

    // "press ENTER to ___" hint
    const char* action   = play_active ? "play" : "go to settings";
    std::string hint     = std::string("press ENTER to ") + action;
    float pulse = 0.55f + 0.3f * std::sin(anim_time * 2.5f);
    draw_text(font, hint.c_str(), -get_text_width(font, hint.c_str(), 0.028f) / 2.0f, -0.72f, 0.028f, pulse, pulse, pulse);

    // Transitions
    if (check_text_click(window, "PLAY",     fb_w, fb_h, cur_aspect)) return SceneID::Game;
    if (check_text_click(window, "SETTINGS", fb_w, fb_h, cur_aspect)) return SceneID::Settings;

    bool enter_now = is_key_down(window, GLFW_KEY_ENTER);
    if (enter_now && !enter_was_down) {
        enter_was_down = enter_now;
        return play_active ? SceneID::Game : SceneID::Settings;
    }
    enter_was_down = enter_now;

    return SceneID::None;
}
