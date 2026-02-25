#ifndef RENDERING_H
#define RENDERING_H

#include "allocator.h"

DrawData* createobj(const float* vertex_c, int vertex_count, float X, float Y,
                    float R, float G, float B);

unsigned int draw_image(const char* filepath, float x, float y, float w, float h, float* out_corrected_w = nullptr);
void draw_text(const char* font_path, const char* text, float x, float y, float size, float r, float g, float b);
float get_text_cap_height(const char* font_path, float text_size);
float get_text_width(const char* font_path, const char* text, float text_size);

#endif
