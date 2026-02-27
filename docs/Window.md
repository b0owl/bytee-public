### Initialization
Calling `init_bytee` will run `glfwInit()` to initialize GLFW.
For the window setup/boilerplate, you have the following functions at your disposal:

### Window Creation
**`create_window(int x_dim, int y_dim, const char* title_bar)`**    
`x_dim` - x dimension of the window in pixels  
`y_dim` - y dimension of the window in pixels  
`title_bar` - window title  
Creates a window and returns a GLFWwindow pointer  

### Setup
**`setup_2d_orthographic(GLFWwindow* window, std::array<float, 4> clrcolor)`**    
Used for setting up a default 2D orthographic camera.  
Returns the window's aspect ratio, or `-1.0f` on error.  

**`apply_optimized_2d_settings()`**
Sets the following window hints:
```cpp
glfwWindowHint(GLFW_SAMPLES,                0);
glfwWindowHint(GLFW_DOUBLEBUFFER,           GLFW_TRUE);
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,  3);
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,  3);
glfwWindowHint(GLFW_OPENGL_PROFILE,         GLFW_OPENGL_CORE_PROFILE);
glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,  GLFW_TRUE);
glfwWindowHint(GLFW_REFRESH_RATE,           GLFW_DONT_CARE);
glfwWindowHint(GLFW_STEREO,                 GLFW_FALSE);
glfwWindowHint(GLFW_SRGB_CAPABLE,           GLFW_FALSE);
glfwWindowHint(GLFW_DEPTH_BITS,             0);
glfwWindowHint(GLFW_STENCIL_BITS,           0);
```

**`apply_optimized_legacy_2d_settings()`**
Sets the following window hints:
```cpp
glfwWindowHint(GLFW_SAMPLES,        0);
glfwWindowHint(GLFW_DOUBLEBUFFER,   GLFW_TRUE);
glfwWindowHint(GLFW_REFRESH_RATE,   GLFW_DONT_CARE);
glfwWindowHint(GLFW_STEREO,         GLFW_FALSE);
glfwWindowHint(GLFW_SRGB_CAPABLE,   GLFW_FALSE);
glfwWindowHint(GLFW_DEPTH_BITS,     0);
glfwWindowHint(GLFW_STENCIL_BITS,   0);
```

**`Other window hint settings:`**
```cpp
enable_msaa_4x()                              
enable_msaa_8x()                         
enable_double_buffering()                  
set_refresh_rate(int refresh)              
set_clear_color(float R, float G, float B, float A)
```

### Housekeeping
**`glCleanup(GLFWwindow *window)`**
House-keeping to run at the end of your mainloop
```cpp
glfwSwapBuffers(window);
glfwPollEvents();
glClear(GL_COLOR_BUFFER_BIT);
```

**`update_viewport(GLFWwindow* window, int* fb_w, int* fb_h, float* aspect)`**  
`fb_w` - framebuffer width in pixels
`fb_h` - framebuffer height in pixels
`aspect` - fb_w / fb_h  
Queries for the current framebuffer size, updates the 2d orthographic projection and GL viewport,
and writes the results to the output params. Call along with your other mainloop housekeeping.

### Widgeting 
**`WidgetArea (STRUCT)`**  
args:  
&nbsp;&nbsp;&nbsp;&nbsp;`floats x1, x2`   
&nbsp;&nbsp;&nbsp;&nbsp;`floats y1, y2`

**`WidgetInfotArea (STRUCT)`**  
args:  
&nbsp;&nbsp;&nbsp;&nbsp;`float x`  
&nbsp;&nbsp;&nbsp;&nbsp;`float y`

**`define_widget_area(const char* ID, FLOATS -> x1, x2, y1, y2)`**  
`ID` - A string that allows tracking widgets  
`x1, x2, y1, y2` - The widget area  
Returns a WidgetArea object with the area of the widget.  

**`get_widget_area(const char* ID)`**  
`ID` - ID to lookup  
Returns the WidgetArea object if found, otherwise it returns -2.0f for x1, x2, y1, and y2 (-2.0 is guaranteed to be off-screen)

**`check_widget_hover(GLFWwindow* window, const char* ID, int fb_w, int fb_h, float cur_aspect)`**  
`window` - GLFWwindow pointer  
`ID` - ID to lookup  
`fb_w` - framebuffer width  
`fb_h` - framebuffer height  
`cur_aspect` - fb_w / fb_h  
Returns `true` if mouse is hovering on a given widget, otherwise it will return `false`.

**`check_widget_click(GLFWwindow* window, const char* ID, int fb_w, int fb_h, float cur_aspect)`**  
`window` - GLFWwindow pointer  
`ID` - ID to lookup  
`fb_w` - framebuffer width  
`fb_h` - framebuffer height  
`cur_aspect` - fb_w / fb_h  
Returns `1` if user left-clicks a widget, `2` if a user right-clicks a widget, and `3` if a user middle-clicks a widget.

**`optimal_widget_info_area(const char* ID, int fb_w, int fb_h, float text_size, const char* font_path, const char* text)`**
`ID` - ID of the widget to place the tooltip near
`fb_w` - framebuffer width
`fb_h` - framebuffer height
`text_size` - size of the tooltip text (same value you'll pass to `draw_text`)
`font_path` - path to the font (same value you'll pass to `draw_text`)
`text` - the tooltip string
Returns a `WidgetInfoArea` with the `x, y` position to draw the tooltip. Places it to the right of the widget, or to the left if the widget is near the right edge of the screen. Vertically centered on the widget.

**`define_text_area(const char* ID, const char* font_path, const char* text, float x, float y, float size)`**
`ID` - ID to register this text area under
`font_path` - path to the font used for rendering the text
`text` - the string being rendered
`x` - left edge in world coordinates (same x passed to `draw_text`)
`y` - baseline in world coordinates (same y passed to `draw_text`)
`size` - text size in world units (same value passed to `draw_text`)
Returns a `WidgetArea` with the bounding box of the text, computed from font measurements. Registers it under `ID` so `check_widget_hover` / `check_widget_click` work on it.