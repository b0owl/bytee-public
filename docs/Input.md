### Keyboard Input

**`is_key_down(GLFWwindow* window, int key)`**
`window` - window pointer
`key` - GLFW key constant (e.g. `GLFW_KEY_LEFT`)
Returns `true` every frame the key is held down. Use this for continuous input (movement, held actions).

**`is_key_up(GLFWwindow* window, int key)`**
`window` - window pointer
`key` - GLFW key constant
Returns `true` when the key is not pressed.

### Edge-triggered key events

For single press detection (not continuous), use the key callback system. It's a single-header — define `KEYBOARD_IMPL` in exactly one `.cpp` before including:

```cpp
// in one .cpp file only:
#define KEYBOARD_IMPL
#include "keyboard.h"

// register once at startup:
glfwSetKeyCallback(window, key_callback);
```

Each physical key press/release sets the globals `kb_key` and `kb_action`. Check them each frame:

```cpp
if (kb_action == GLFW_PRESS && kb_key == GLFW_KEY_ENTER) {
    // handle single press
}
kb_key = 0; kb_action = 0; // clear after handling
```

The difference from `is_key_down`: the callback fires once per event, `is_key_down` fires every frame the key is held.

---

### Mouse Input

**`MouseState (STRUCT)`**
&nbsp;&nbsp;&nbsp;&nbsp;`double x, y` - cursor position in screen pixels
&nbsp;&nbsp;&nbsp;&nbsp;`bool left_button` - left mouse button held
&nbsp;&nbsp;&nbsp;&nbsp;`bool right_button` - right mouse button held
&nbsp;&nbsp;&nbsp;&nbsp;`bool middle_button` - middle mouse button held

**`get_mouse_state(GLFWwindow* window, MouseState& mouse)`**
`window` - window pointer
`mouse` - MouseState to write into
Fills `mouse` with the current cursor position and button states.

```cpp
MouseState ms;
get_mouse_state(window, ms);
if (ms.left_button) { /* left is held */ }
```

For edge-triggered click detection (single click, not held), track the previous state yourself:

```cpp
bool just_clicked = ms.left_button && !prev_mouse_down;
prev_mouse_down   = ms.left_button;
```
