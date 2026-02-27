### Scene System

The scene system lets you split your application into discrete states (title screen, gameplay, settings, etc.) and switch between them cleanly.

### Scene

Every scene inherits from the `Scene` base struct and implements `tick()`:

```cpp
struct Scene {
    virtual ~Scene() = default;
    virtual SceneID tick(GLFWwindow* window, float delta_time,
                         float cur_aspect, int fb_w, int fb_h) = 0;
};
```

`tick()` is called once per frame. It should do all rendering and logic for that scene, then return:
- `SceneID::None` — stay on this scene
- Any other `SceneID` — request a transition to that scene

### SceneID

```cpp
enum class SceneID { None, Title, Game, Settings };
```

Add values here as you add scenes to your project.

### SceneManager

Owns the currently active scene and delegates `tick()` to it.

**`set(std::unique_ptr<Scene> scene)`**
Replaces the active scene. The old scene is destroyed immediately.

**`tick(GLFWwindow* window, float delta_time, float cur_aspect, int fb_w, int fb_h)`**
Calls `tick()` on the active scene and returns its result. Returns `SceneID::None` if no scene is set.

### Usage

```cpp
SceneManager scene_manager;
scene_manager.set(std::make_unique<TitleScreen>());

while (!glfwWindowShouldClose(window)) {
    // ...

    SceneID next = scene_manager.tick(window, delta_time, cur_aspect, fb_w, fb_h);

    if      (next == SceneID::Game)     scene_manager.set(std::make_unique<GameScene>());
    else if (next == SceneID::Title)    scene_manager.set(std::make_unique<TitleScreen>());
    else if (next == SceneID::Settings) scene_manager.set(std::make_unique<SettingsScene>());

    glCleanup(window);
}
```

Each scene transition creates a fresh instance — no state is carried over between switches.

### Adding a new scene

1. Add a value to `SceneID` in `scene.h`
2. Create your class inheriting `Scene`, implement `tick()`
3. In your main loop, handle the new `SceneID` with `scene_manager.set(...)`
