### Spritesheet Slicing

The spritesheet API lets you load a grid-based spritesheet and render individual frames from it.

### SpriteSheet (struct)
&nbsp;&nbsp;&nbsp;&nbsp;`unsigned int tex` - GL texture ID
&nbsp;&nbsp;&nbsp;&nbsp;`int img_w, img_h` - full image dimensions in pixels
&nbsp;&nbsp;&nbsp;&nbsp;`int cols, rows` - grid dimensions

### load_spritesheet

**`load_spritesheet(const char* filepath, int cols, int rows)`**
`filepath` - path to the image file
`cols` - number of columns in the grid
`rows` - number of rows in the grid

Loads the image and uploads it to the GPU. Subsequent calls with the same filepath return the cached texture without reloading from disk.
Returns a `SpriteSheet` to pass to `draw_sprite`.

### draw_sprite

**`draw_sprite(SpriteSheet sheet, int frame, float x, float y, float w, float h, float* out_corrected_w)`**
`sheet` - SpriteSheet returned by `load_spritesheet`
`frame` - zero-based frame index, ordered left-to-right then top-to-bottom
`x, y` - bottom-left corner in world coordinates
`w, h` - desired size in world units; width is corrected for the cell's aspect ratio automatically
`out_corrected_w` - optional, written with the actual rendered width

Renders a single frame from the spritesheet.

### Frame indexing

Frames are indexed starting at 0, left-to-right, top-to-bottom:

```
[ 0 ][ 1 ][ 2 ][ 3 ]
[ 4 ][ 5 ][ 6 ][ 7 ]
[ 8 ][ 9 ][10 ][11 ]
```

### Example

```cpp
// Load once (e.g. in constructor or first frame)
SpriteSheet sheet = load_spritesheet("player.png", 4, 3);

// Draw frame 5 each tick
draw_sprite(sheet, 5, -0.1f, -0.1f, 0.2f, 0.2f);

// Animate based on time
int frame = (int)(anim_time * 12.0f) % (sheet.cols * sheet.rows);
draw_sprite(sheet, frame, x, y, 0.2f, 0.2f);
```
