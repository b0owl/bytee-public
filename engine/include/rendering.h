#ifndef RENDERING_H
#define RENDERING_H

#include "allocator.h"

DrawData* createobj(const float* vertex_c, int vertex_count, float X, float Y,
                    float R, float G, float B);

unsigned int draw_image(const char* filepath, float x, float y, float w, float h, float* out_corrected_w = nullptr);
void draw_text(const char* font_path, const char* text, float x, float y, float size, float r, float g, float b);
float get_text_cap_height(const char* font_path, float text_size);
float get_text_width(const char* font_path, const char* text, float text_size);

// Spritesheet slicing
struct SpriteSheet {
    unsigned int tex;
    int img_w, img_h;
    int cols, rows;
};

SpriteSheet load_spritesheet(const char* filepath, int cols, int rows);
void draw_sprite(SpriteSheet sheet, int frame, float x, float y, float w, float h, float* out_corrected_w = nullptr);

#endif
