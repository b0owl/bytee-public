### Images

**`draw_image(const char* filepath, float x, float y, float w, float h, float* out_corrected_w)`**
`filepath` - path to the image file (PNG, JPG, etc.)
`x, y` - bottom-left corner in world coordinates
`w, h` - desired height in world units; width is corrected for the image's aspect ratio automatically
`out_corrected_w` - optional, written with the actual rendered width

Loads the image, uploads it to the GPU, and draws it immediately. Returns the GL texture ID.
Note: each call re-uploads the image. Cache the texture ID yourself if you draw the same image every frame, or use `load_spritesheet` which handles caching internally.

```cpp
float icon_w;
draw_image("icon.png", -0.5f, -0.1f, 0.0f, 0.2f, &icon_w);
// icon_w now holds the corrected width; use it to position things beside the image
```

---

### Fonts

Font atlases are cached internally — each TTF file is baked to a GPU texture once per path. Subsequent calls with the same `font_path` reuse the cached atlas.

**`draw_text(const char* font_path, const char* text, float x, float y, float size, float r, float g, float b)`**
`font_path` - path to a `.ttf` file
`text` - ASCII string to draw (characters outside 32–127 are skipped)
`x, y` - left edge and baseline in world coordinates
`size` - glyph height in world units (e.g. `0.05` = 5% of screen height)
`r, g, b` - text color, each in [0, 1]

Renders the string using the given font. `y` is the **baseline** — glyphs extend upward from it. Use `get_text_cap_height` to find the visible height above the baseline.

```cpp
draw_text("font.ttf", "Hello", -0.4f, -0.1f, 0.08f, 1.0f, 1.0f, 1.0f);
```

---

**`get_text_cap_height(const char* font_path, float text_size)`**
`font_path` - same path passed to `draw_text`
`text_size` - same size value passed to `draw_text`

Returns the ascender height (visual cap height) of the font in world units at the given size. Use this for vertical centering — `draw_text` places `y` at the baseline, so text extends upward by this amount, not by the full `text_size`.

```cpp
float cap = get_text_cap_height("font.ttf", 0.08f);
float y = center_y - cap / 2.0f;  // vertically center text around center_y
draw_text("font.ttf", "Score", x, y, 0.08f, 1, 1, 1);
```

---

**`get_text_width(const char* font_path, const char* text, float text_size)`**
`font_path` - same path passed to `draw_text`
`text` - the string to measure
`text_size` - same size value passed to `draw_text`

Returns the total advance width of the string in world units. Use this to right-align or horizontally center text.

```cpp
float w = get_text_width("font.ttf", "Score: 9999", 0.06f);
float x = center_x - w / 2.0f;  // horizontally center text
draw_text("font.ttf", "Score: 9999", x, y, 0.06f, 1, 1, 1);
```
