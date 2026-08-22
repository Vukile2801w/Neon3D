# Neon3D Engine — API Reference

Neon3D is a lightweight C++ game engine built on **OpenGL 4.6 (core profile)**, **GLFW**, **GLAD**, **GLM**, and **stb_image**. This document covers the public API of the `Neon` namespace: windowing, input, timing, cameras, transforms, shaders, textures, materials, and mesh/buffer objects.

> Generated from source in `Engine/include/` and `Engine/src/`, cross-checked against `Sandbox/src/main.cpp`. Windows is currently the only tested build target (CMake links `opengl32`, `gdi32`, `user32`, `shell32`).

---

## Table of Contents

- [Getting Started](#getting-started)
- [Window](#window)
- [Input](#input)
- [Time](#time)
- [Camera](#camera)
- [Transform](#transform)
- [Shader](#shader)
- [Texture](#texture)
- [Material](#material)
- [Mesh](#mesh)
  - [VertexBuffer](#vertexbuffer)
  - [VertexArray](#vertexarray)
  - [IndexBuffer](#indexbuffer)
- [Logging](#logging)
- [Full Example](#full-example)

---

## Getting Started

Neon3D is built with CMake as a static library (`Engine`) that a consumer target (e.g. `Sandbox`) links against.

```cmake
add_subdirectory(Engine)
add_subdirectory(Sandbox)   # your own target

target_link_libraries(Sandbox PRIVATE Engine)
```

A minimal program:

```cpp
#include "Window.hpp"
#include "Time.hpp"
#include "Input.hpp"

int main()
{
    Neon::Window window;              // 800x600 by default
    Neon::Input input(&window);
    Neon::Time time;

    while (!window.shouldWindowsClose())
    {
        time.beginFrame();
        input.handleInput();

        // ... update & draw calls here ...

        window.render();
    }
}
```

---

## Window

`#include "Window.hpp"`

Owns the GLFW window and OpenGL context. Creating a `Window` initializes GLFW, creates an OpenGL 4.6 core-profile context, and loads GLAD. Depth testing (`GL_DEPTH_TEST`) is enabled automatically.

```cpp
Window();
Window(unsigned int xSize, unsigned int ySize);
```
Constructs and opens a window titled `"Neon3D"`. The no-argument overload defaults to **800×600**. If GLFW/GLAD initialization fails, an error is logged and the window is left in an unusable state (check `shouldWindowsClose()`).

In addition to depth testing, the constructor now enables **back-face culling** (`GL_CULL_FACE`, cull `GL_BACK`, front face `GL_CCW`) — meshes must use counter-clockwise winding order for their front-facing triangles, or they'll be invisible.

```cpp
void render();
```
Swaps the front/back buffers and clears the color + depth buffers for the next frame. Call this **once per frame, after** your draw calls.

```cpp
bool shouldWindowsClose();
```

```cpp
int getWidth();
int getHeight();
float getAspectRatio();   // width / height, updated on resize
```

```cpp
GLFWwindow* getGlfwWindow();
```
Escape hatch to the raw GLFW handle, used internally by `Input`.

**Resizing:** The engine registers a framebuffer-resize callback automatically; `getWidth()`/`getHeight()`/`getAspectRatio()` stay in sync without any action needed from you.

---

## Input

`#include "Input.hpp"`

Polls keyboard and mouse state once per frame. Must be constructed with a pointer to an existing `Window`.

```cpp
Input(Window *window);
void handleInput();   // call once per frame, before reading state
```

### Keyboard

```cpp
bool isKeyDown(Key key);       // true every frame the key is held
bool isKeyPressed(Key key);    // true only on the frame it was pressed
bool isKeyReleased(Key key);   // true only on the frame it was released
```

`Key` is an enum covering `KEY_A`–`KEY_Z`, `KEY_0`–`KEY_9`, arrows, modifiers (`KEY_SHIFT`, `KEY_CTRL`, `KEY_ALT`), `KEY_F1`–`KEY_F12`, and common control keys (`KEY_SPACE`, `KEY_ENTER`, `KEY_ESCAPE`, etc.). Only left-hand modifier keys are mapped (`KEY_SHIFT` → left shift, etc.).

### Mouse

```cpp
bool isMouseButtonDown(MouseButton key);
bool isMouseButtonPressed(MouseButton key);
bool isMouseButtonReleased(MouseButton key);
float getMouseScrollValue();
```

`MouseButton` covers `MOUSE_BUTTON_LEFT/RIGHT/MIDDLE/4/5`. Scroll-wheel support is declared in the API but **not yet wired up** — `scroll_callback()` in `Input.cpp` is a stub (`// TODO`), so `getMouseScrollValue()` currently always returns `0`.

`operator<<` is overloaded for both `Key` and `MouseButton`, so you can `std::cout << someKey` for debug logging.

---

## Time

`#include "Time.hpp"`

Simple frame-timing utility built on `std::chrono::steady_clock`.

```cpp
Time();
void beginFrame();          // call once per frame, before using delta/elapsed values
float getDeltaTime();       // seconds since the previous beginFrame() call
double sinceStart();        // seconds since the Time object was constructed
double sinceStartOfFrame(); // seconds since the current frame began
double sinceLastFrame();    // seconds since the previous frame ended
TimePoint now();            // raw std::chrono time point
```

---

## Camera

`#include "Camera.hpp"`

A simple free-fly camera (no scene graph / parenting).

```cpp
glm::vec3 position{0, 0, 3};
glm::vec3 rotation{0};   // Euler angles in radians: pitch (x), yaw (y), roll (z)
float FOV{45.0f};        // degrees

glm::mat4 getViewMatrix() const;
glm::mat4 getProjectionMatrix(float aspectRatio) const;
glm::vec3 getForward() const;
glm::vec3 getRight() const;
```

`getProjectionMatrix` uses a fixed near/far clip of **0.1 / 100.0**. `rotation` is applied yaw → pitch → roll when building the view matrix. `position`, `rotation`, and `FOV` are public fields — mutate them directly (see the Sandbox example for camera-fly-around controls).

---

## Transform

`#include "Transform.hpp"`

Position/rotation/scale container with a matrix builder. No parent/child hierarchy.

```cpp
Transform(glm::vec3 position = {0}, glm::vec3 rotation = {0}, glm::vec3 scale = {1});

glm::vec3 position{0.0f};
glm::vec3 rotation{0.0f};  // Euler radians, applied X then Y then Z
glm::vec3 scale{1.0f};

glm::mat4 getMatrix() const;  // translate * rotateX * rotateY * rotateZ * scale
```

---

## Shader

`#include "Shader.hpp"`

Wraps an OpenGL shader program (one vertex + one fragment stage).

```cpp
Shader(const std::filesystem::path &vertex, const std::filesystem::path &fragment);
```
Loads, compiles, and links a shader program. **Pass an empty path (`""`) for either argument to use that stage's built-in default shader** — a passthrough vertex shader or a solid-orange fragment shader — useful for quick prototyping.

```cpp
void bind() const;
static void unbind();
```

### Uniform setters
```cpp
void setBool(const std::string &name, bool value) const;
void setInt(const std::string &name, int value) const;
void setUInt(const std::string &name, unsigned int value) const;
void setFloat(const std::string &name, float value) const;
void setVec2(const std::string &name, float x, float y) const;
void setVec3(const std::string &name, float x, float y, float z) const;
void setVec4(const std::string &name, float x, float y, float z, float w) const;
void setMat3(const std::string &name, const float *matrix) const;
void setMat4(const std::string &name, const float *matrix) const;
```
All setters look up the uniform location internally by name each call — no manual location caching required. The shader must be bound (or these calls no-op against the wrong program) — in practice you'll set uniforms via `Material::set`, which binds for you (see below).

`ShaderDataType` (`Float`, `Int`, `Double`) is used separately, to describe **vertex attribute** layouts (see `VertexArray`/`BufferElement`) — not shader uniform types.

---

## Texture

`#include "Texture.hpp"`

Loads a 2D texture from disk via `stb_image`.

```cpp
Texture(const std::filesystem::path &path);
~Texture();

void setFilter(TextureFilter min, TextureFilter mag);
void setWrap(TextureWrap s, TextureWrap t);

void bind(unsigned int slot = 0) const;
static void unbind();

int width() const;
int height() const;
```

`TextureFilter`: `Linear`, `Nearest`, and mipmap variants (`LinearMipmapLinear`, `LinearMipmapNearest`, `NearestMipmapLinear`, `NearestMipmapNearest`).
`TextureWrap`: `Repeat`, `MirroredRepeat`, `ClampToEdge`, `ClampToBorder`.

---

## Material

`#include "Material.hpp"`

Binds a `Shader` together with a set of named textures and uniform values.

```cpp
Material(Shader &shader);   // holds a reference — the Shader must outlive the Material

void bind() const;          // binds the shader, then binds each texture to a sequential slot
                             // and sets the matching sampler uniform automatically

void setTexture(const std::string &name, Texture &texture);
void set(const std::string &name, MaterialProperty data);

glm::vec3 color;             // convenience field; not applied automatically — pass it via set()
```

`MaterialProperty` is a `std::variant` accepting: `bool`, `int`, `unsigned int`, `float`, `glm::vec2`, `glm::vec3`, `glm::vec4`, `glm::mat3`, `glm::mat4`. `set()` dispatches to the matching `Shader::set*` call based on which type is active in the variant.

```cpp
material.setTexture("T_Color", texture);
material.set("u_Model", transform.getMatrix());
material.set("u_Color", material.color);
```

---

## Mesh

`#include "Mesh/Mesh.hpp"`

Combines a vertex buffer, index buffer, and vertex array into a single drawable object.

```cpp
Mesh(
    const void *vertices, size_t vertexSize,
    const unsigned int *indices, size_t indexSize,
    std::initializer_list<BufferElement> layout);

void draw() const;
```

`layout` describes the interleaved vertex attribute layout, e.g.:
```cpp
{
    {3, ShaderDataType::Float},  // position
    {3, ShaderDataType::Float},  // normal
    {2, ShaderDataType::Float},  // UV
    {3, ShaderDataType::Float},  // tangent
}
```
`BufferElement { int size; ShaderDataType type; }` — `size` is the number of components per attribute (e.g. `3` for a `vec3`).

### VertexBuffer
`#include "Mesh/VertexBuffer.hpp"`
```cpp
VertexBuffer(const void *data, size_t size);
void bind() const;
void unbind() const;
```

### VertexArray
`#include "Mesh/VertexArray.hpp"`
```cpp
VertexArray();
void bind() const;
void unbind() const;
void addBuffer(const VertexBuffer &buffer, std::initializer_list<BufferElement> elements);
```

### IndexBuffer
`#include "Mesh/IndexBuffer.hpp"`
```cpp
IndexBuffer(const void *data, size_t size);
void bind() const;
static void unbind();
```

> In practice you'll rarely touch `VertexBuffer`/`VertexArray`/`IndexBuffer` directly — `Mesh`'s constructor wires all three together for you.

---

## Logging

`#include "Logging.hpp"`

Static, engine-wide colored console logger (used internally by `Window`, but available for your own code too).

```cpp
static void Init(std::string nameSpace);  // resets tag + sets level to Info

static void Info(std::string message);
static void Error(std::string message);

static LoggingLevel level;      // Info | Warning | Error | None
static std::string nameSpace;   // shown as "[nameSpace][LEVEL] - message"
```

Default tag is `"Neon"`, default level is `Info`. Output is ANSI-colored (green/yellow/red) — best viewed in a terminal that supports ANSI escape codes.

---

## Full Example

Condensed from `Sandbox/src/main.cpp` — a spinning, textured, lit cube with fly camera controls:

```cpp
Neon::Window window;
Neon::Input input(&window);
Neon::Time time;

Neon::Camera camera;
camera.position.y = 1.3f;
camera.FOV = 60;

Neon::Shader shader("shader.vert", "shader.frag");
Neon::Material material(shader);
Neon::Texture texture("brick.jpg");
material.setTexture("T_Color", texture);

Neon::Mesh mesh(vertices, sizeof(vertices), indices, sizeof(indices),
    {{3, Neon::ShaderDataType::Float},   // position
     {3, Neon::ShaderDataType::Float},   // normal
     {2, Neon::ShaderDataType::Float}}); // UV

Neon::Transform transform;

while (!window.shouldWindowsClose())
{
    time.beginFrame();
    input.handleInput();
    float dt = time.getDeltaTime();

    if (input.isKeyDown(Neon::Input::Key::KEY_W))
        camera.position += camera.getForward() * 5.0f * dt;

    material.bind();
    material.set("u_Model", transform.getMatrix());
    material.set("u_View", camera.getViewMatrix());
    material.set("u_Projection", camera.getProjectionMatrix(window.getAspectRatio()));
    mesh.draw();

    window.render();
}
```

---

*Third-party libraries bundled under `Engine/lib/` (GLFW, GLAD, GLM, stb_image) are not covered here — see their own upstream docs.*