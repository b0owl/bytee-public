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

#ifdef _WIN32
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#include <GLFW/glfw3.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "../vendor/stb_image.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../vendor/stb_truetype.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "../include/allocator.h"

/*
@brief, Creates and returns an object with the information specified 
        upon calling this function. It will not store the object in 
        the memory allocator, you must do this manually by running
        ALLOCATOR.store_ptr(INDEX, createobj(ARGS)). Note that vertex_c
        is taken as a pointer to a list, such that a vector is not required

@param vertex_c, array of vertex coordinates (not to be confused with X,Y args),
       these only specify the distance from one vertex to another

@param vertex_count,  number of verticies
@param X, Starting X coordinate
@param Y, Starting Y coordinate
@param R/G/B, RGB values
*/
DrawData* createobj(const float* vertex_c, int vertex_count, float X, float Y, 
                    float R, float G, float B) 
{
    DrawData* sprite = new DrawData{};
    sprite->vertex_count = vertex_count;
    
    // Copy all vertex data (vertex_count * 2 floats)
    for (int i = 0; i < vertex_count * 2; i++) {
        sprite->vertices[i] = vertex_c[i];
    }
    
    sprite->r = R;
    sprite->g = G;
    sprite->b = B;
    sprite->x = X;
    sprite->y = Y;
    
    return sprite;
}

unsigned int draw_image(const char* filepath, float x, float y, float w, float h, float* out_corrected_w) {
    int img_w, img_h, channels;
    unsigned char* data = stbi_load(filepath, &img_w, &img_h, &channels, 0);
    if (!data) return 0;

    // Correct for image aspect ratio.
    // glOrtho(-aspect, aspect, -1, 1, ...) makes 1 world-unit equal in both axes,
    // so no viewport aspect correction is needed here.
    float img_aspect = (float)img_w / (float)img_h;
    float corrected_w = w * img_aspect;
    if (out_corrected_w) *out_corrected_w = corrected_w;

    unsigned int tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // prevent row-shearing on non-4-byte-aligned RGB images
    glTexImage2D(GL_TEXTURE_2D, 0, format, img_w, img_h, 0, format, GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    stbi_image_free(data);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.0f, 1.0f); glVertex2f(x,               y);
        glTexCoord2f(1.0f, 1.0f); glVertex2f(x + corrected_w, y);
        glTexCoord2f(1.0f, 0.0f); glVertex2f(x + corrected_w, y + h);
        glTexCoord2f(0.0f, 0.0f); glVertex2f(x,               y + h);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    return tex;
}

// --- Text rendering ----------------------------------------------------------

static const int ATLAS_SIZE  = 512;
static const float BAKE_SIZE = 64.0f;  // font is baked at this px height

struct BakedFont {
    unsigned int tex;
    stbtt_bakedchar chars[96];  // ASCII 32–127
    float pixel_ascender;       // ascender in pixels at BAKE_SIZE
};


// sorry about readability
// Store BakedFont as heap pointer so the address is stable across map rehashes.
static std::unordered_map<std::string, BakedFont*> font_cache;

static BakedFont* load_font(const char* font_path) {
    auto it = font_cache.find(font_path);
    if (it != font_cache.end()) return it->second;

    FILE* f = fopen(font_path, "rb");
    if (!f) return nullptr;
    fseek(f, 0, SEEK_END);
    std::vector<unsigned char> buf(ftell(f));
    rewind(f);
    fread(buf.data(), 1, buf.size(), f);
    fclose(f);

    std::vector<unsigned char> bitmap(ATLAS_SIZE * ATLAS_SIZE);
    BakedFont* baked = new BakedFont();
    stbtt_BakeFontBitmap(buf.data(), 0, BAKE_SIZE, bitmap.data(),
                         ATLAS_SIZE, ATLAS_SIZE, 32, 96, baked->chars);

    stbtt_fontinfo info;
    stbtt_InitFont(&info, buf.data(), 0);
    int asc, desc, lg;
    stbtt_GetFontVMetrics(&info, &asc, &desc, &lg);
    float font_scale = stbtt_ScaleForPixelHeight(&info, BAKE_SIZE);
    baked->pixel_ascender = asc * font_scale;

    // Expand 1-channel bitmap to RGBA — alpha = bitmap value, RGB = 255.
    // GL_ALPHA as internal format is unreliable on macOS; GL_RGBA is not.
    std::vector<unsigned char> rgba(ATLAS_SIZE * ATLAS_SIZE * 4);
    for (int i = 0; i < ATLAS_SIZE * ATLAS_SIZE; i++) {
        rgba[i*4+0] = rgba[i*4+1] = rgba[i*4+2] = 255;
        rgba[i*4+3] = bitmap[i];
    }

    glGenTextures(1, &baked->tex);
    glBindTexture(GL_TEXTURE_2D, baked->tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, ATLAS_SIZE, ATLAS_SIZE,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    font_cache[font_path] = baked;
    return baked;
}

/*
@brief, renders a string of text using a TTF font.
        Font atlases are cached — the TTF is only loaded once per path.

@param font_path, path to a .ttf file
@param text, the string to draw
@param x/y, bottom-left origin in GL world coordinates
@param size, height in world units (e.g. 0.05 = 5% of screen height)
@param r/g/b, text color
*/
void draw_text(const char* font_path, const char* text,
               float x, float y, float size, float r, float g, float b) {
    BakedFont* font = load_font(font_path);
    if (!font) return;

    float scale = size / BAKE_SIZE;

    // Precompute all quads before touching GL state — stbtt must not be
    // called between glBegin/glEnd (undefined behaviour on macOS drivers).
    stbtt_aligned_quad quads[128];
    int quad_count = 0;
    float cx = 0.0f, cy = 0.0f;
    for (const char* p = text; *p && quad_count < 128; p++) {
        if (*p < 32 || *p > 127) continue;
        stbtt_GetBakedQuad(font->chars, ATLAS_SIZE, ATLAS_SIZE,
                           *p - 32, &cx, &cy, &quads[quad_count++], 1);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, font->tex);
    glColor3f(r, g, b);

    glBegin(GL_QUADS);
    for (int i = 0; i < quad_count; i++) {
        const stbtt_aligned_quad& q = quads[i];
        // convert pixel offsets to world coords; flip Y (stbtt is top-down)
        glTexCoord2f(q.s0, q.t0); glVertex2f(x + q.x0 * scale, y - q.y0 * scale);
        glTexCoord2f(q.s1, q.t0); glVertex2f(x + q.x1 * scale, y - q.y0 * scale);
        glTexCoord2f(q.s1, q.t1); glVertex2f(x + q.x1 * scale, y - q.y1 * scale);
        glTexCoord2f(q.s0, q.t1); glVertex2f(x + q.x0 * scale, y - q.y1 * scale);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}

/*
@brief, returns the visual cap height (ascender height) of text in world units.
        Use this for accurate vertical centering — draw_text places y at the
        baseline, so text extends upward by this amount, not by the full text_size.

@param font_path, same path passed to draw_text
@param text_size, same size value passed to draw_text
*/
float get_text_cap_height(const char* font_path, float text_size) {
    BakedFont* font = load_font(font_path);
    if (!font) return text_size;
    return font->pixel_ascender * (text_size / BAKE_SIZE);
}

float get_text_width(const char* font_path, const char* text, float text_size) {
    BakedFont* font = load_font(font_path);
    if (!font) return 0.0f;
    float scale = text_size / BAKE_SIZE;
    float width = 0.0f;
    for (const char* p = text; *p; p++) {
        if (*p < 32 || *p > 127) continue;
        width += font->chars[*p - 32].xadvance * scale;
    }
    return width;
}