# Contributing to Neon3D

This document describes how development on Neon3D is actually done today, and the conventions new code should follow so the engine doesn't grow more inconsistent than it already is. It is based on a direct inspection of the current repository — not a generic C++ style guide.

Where the existing code is inconsistent, this document says so explicitly, picks one convention as the standard for **new** code, and does not silently pretend the codebase already follows it.

---

## 1. Project Overview

Neon3D is a **C++17** game engine built on:

- **Graphics API:** OpenGL 4.6 (core profile)
- **Windowing/input backend:** GLFW (vendored as a prebuilt static library, `libglfw3.a`)
- **GL function loading:** GLAD (vendored, built via its own `CMakeLists.txt`)
- **Math:** GLM (vendored, header-only)
- **Image loading:** stb_image (vendored, header-only, single-file)
- **Build system:** CMake, two targets — `Engine` (static library) and `Sandbox` (executable consumer)
- **Current platform:** Windows/MinGW. The vendored GLFW library is a prebuilt Windows `.a` file, and `Engine/CMakeLists.txt` links `opengl32`, `gdi32`, `user32`, `shell32` — all Windows system libraries. Neon3D is **not** currently cross-platform; that's a stated long-term goal, not a current property.

**Engine vs. Sandbox:** `Engine` is a static library with no `main()` — it exposes engine systems under the `Neon` namespace. `Sandbox` is a consumer executable that links against `Engine` and is currently the only example/test of the public API. There is no other consumer project yet.

**What exists today:** `Window`, `Input`, `Time`, `Logging`, `Camera`, `Transform`, `Shader`, `Texture`, `Material`, and the `Mesh`/`VertexArray`/`VertexBuffer`/`IndexBuffer` group. There is **no** `Renderer` class, **no** event system, **no** `Application` class, **no** scene/entity system, **no** resource/asset manager, and **no** physics/audio/gameplay/editor systems. Don't write documentation or comments implying these exist — they don't yet.

---

## 2. Repository Structure

As currently laid out (this snapshot did not include a root `CMakeLists.txt` — only `Engine/CMakeLists.txt` and `Sandbox/CMakeLists.txt` exist; if a root file ties them together in your working copy, keep it minimal — an `add_subdirectory()` for each):

```
Engine/
    include/            # public headers, consumed by Sandbox and any future consumer
        Camera.hpp
        Input.hpp
        Logging.hpp
        Material.hpp
        Shader.hpp
        Texture.hpp
        Time.hpp
        Transform.hpp
        Window.hpp
        Mesh/
            IndexBuffer.hpp
            Mesh.hpp
            VertexArray.hpp
            VertexBuffer.hpp
    src/                # implementation files, mirrors include/ structure
        Camera.cpp
        Input.cpp
        Logging.cpp
        Material.cpp
        Shader.cpp
        Texture.cpp
        Time.cpp
        Transform.cpp
        Window.cpp
        Mesh/
            IndexBuffer.cpp
            Mesh.cpp
            VertexArray.cpp
            VertexBuffer.cpp
    lib/                # vendored third-party dependencies
        glad/           # GLAD loader, has its own CMakeLists.txt
        glfw/           # prebuilt libglfw3.a + headers
        glm/            # header-only
        stb/            # header-only (stb_image.h)
    CMakeLists.txt

Sandbox/
    src/
        main.cpp
    shaders/
        shader.vert
        shader.frag
    assets/
        brick.jpg
        brickNormal.png
    CMakeLists.txt
```

**Where things go:**
- **Public headers** → `Engine/include/`, grouped into a subfolder (like `Mesh/`) only when a system has multiple tightly-related headers. Single-header systems (`Camera`, `Window`, `Shader`, etc.) sit flat in `include/`.
- **Implementation files** → `Engine/src/`, mirroring the `include/` layout exactly (this is already followed consistently).
- **Rendering/core systems** → `Engine/include` + `Engine/src`, same as everything else — there's no separate "core" vs. "rendering" subfolder split yet. If the engine grows a `Renderer`, `Application`, or event system, decide then whether subfolders (`Core/`, `Rendering/`, `Platform/`) are warranted; don't add empty subfolder scaffolding preemptively.
- **Third-party/vendored code** → `Engine/lib/`, one folder per dependency. Never edit vendored code in place.
- **Sandbox/application code** → `Sandbox/src/`, `Sandbox/shaders/`, `Sandbox/assets/`. Sandbox is a consumer of the public `Engine` API, not a place to add reusable engine functionality (see §15).

---

## 3. C++ Coding Conventions

### Naming

| Element | Convention | Evidence |
|---|---|---|
| Namespace | `Neon` (single, all engine code) | Used consistently everywhere |
| Classes | `PascalCase` | `Window`, `Shader`, `VertexArray`, etc. — consistent |
| Structs | `PascalCase` | `BufferElement`, `KeyData`, `MouseButtonData`, `Light` (Sandbox) — consistent |
| Methods/member functions | `camelCase` | `getWidth()`, `bind()`, `setFloat()`, `getViewMatrix()` — consistent |
| Free functions | `camelCase` | `toGLTextureFilter()`, `getTypeSize()` — **mostly** consistent, see inconsistency below |
| Member variables (private/protected) | `m_` + `camelCase` | `m_program`, `m_id`, `m_window`, `m_indexCount` — mostly consistent, see inconsistency below |
| Member variables (public, POD-like data) | `camelCase`, no prefix | `Camera::position/rotation/FOV`, `Transform::position/rotation/scale`, `Material::color` |
| Constants / enum values | inconsistent — see below | |
| Enum types | inconsistent — see below | |
| Files | match the primary class name, `PascalCase.hpp`/`.cpp` | `Shader.hpp`/`Shader.cpp` |

**Inconsistency — `enum` vs. `enum class`:** `ShaderDataType`, `TextureFilter`, `TextureWrap`, and `LoggingLevel` are all scoped `enum class`. `Input::Key` and `Input::MouseButton` are plain, unscoped `enum`. **Standard for new code: use `enum class`** — it's the majority pattern, avoids polluting the enclosing scope, and is safer. Don't rewrite `Input::Key`/`MouseButton` just for this; changing them is a breaking API change with its own cost (see §7 for why `Key`/`MouseButton` values are also format-checked against a `KEY_COUNT`-style sentinel — that pattern would need to move with any rewrite).

**Inconsistency — enum value casing:** `Input::Key`/`MouseButton` values are `UPPER_SNAKE_CASE` (`KEY_A`, `MOUSE_BUTTON_LEFT`), matching GLFW's own naming style. Every other enum's values are `PascalCase` (`ShaderDataType::Float`, `TextureFilter::Linear`, `LoggingLevel::Warning`). **Standard for new code: `PascalCase` enum values** — it's the majority pattern and matches the rest of the type-naming conventions. `Input`'s enums are the deliberate-looking exception (they read like a 1:1 mirror of GLFW's own key names), so leave them as-is rather than rewriting a public, heavily-used API for casing alone.

**Inconsistency — member variable prefixing:** Every class except one is consistent: private/protected members get `m_`, public data members don't. The exception is `Input`: its **protected** members `keyData` and `mouseStatus` (`Input.hpp`) have no `m_` prefix, while its **private** member `m_window` does. **Standard for new code: `m_` prefix applies to any non-public member variable**, protected included. Don't rename `keyData`/`mouseStatus` retroactively — just don't repeat the gap in new code.

**Inconsistency — snake_case in a couple of GLFW-adjacent functions:** `Window::framebuffer_size_callback` (a private static member) and the free function `scroll_callback` in `Input.cpp` both use `snake_case`, breaking from the `camelCase` convention used everywhere else. This looks like it came from copying GLFW-callback-naming habits from tutorials/examples. **Standard for new code: `camelCase`, even for GLFW callback functions** (e.g. `framebufferSizeCallback`). Don't rename the existing two just for style — they're internal, not public API, and the cost of touching working GLFW callback wiring isn't worth it for a naming nit.

### Headers

- **Include guards, not `#pragma once`.** Every header in the codebase uses `#ifndef`/`#define`/`#endif` guards; none use `#pragma once`. Keep using include guards for new headers.
- **Inconsistency — guard naming:** 12 of 13 headers use a `NEON_<NAME>` guard (`NEON_WINDOW`, `NEON_SHADER`, `NEON_VERTEX_BUFFER`, etc.). `Input.hpp` alone uses `INPUT_HPP`. **Standard for new code: `NEON_<NAME_IN_CAPS>`**, matching the dominant pattern. Don't bother renaming `Input.hpp`'s guard — it's cosmetic and not worth an unnecessary diff.
- **Inconsistency — include ordering:** there is currently no single consistent order across `.cpp` files. Examples: `Window.cpp` puts its own header first, then an engine header (`Logging.hpp`), then third-party (`glad`/`glfw`). `Shader.cpp` puts `Logging.hpp` *before* its own `Shader.hpp`. `Input.cpp` and `Mesh.cpp` put third-party (`glad`/`glfw`) *before* their own header. `Material.cpp` puts a standard header before its own header with no third-party involved at all. **Standard for new code**, in this order:
  1. The file's own header (proves the header is self-contained)
  2. C++ standard library headers
  3. Other Neon3D engine headers
  4. Third-party headers (`glad/glad.h`, `glfw/glfw3.h`, `glm.hpp`, `stb_image.h`, etc.)

  Don't reorder existing files' includes just to match this — only apply it going forward, and only touch an existing file's include order if you're already editing that file for another reason.
- **Forward declarations:** used exactly once today — `Window.hpp` forward-declares `struct GLFWwindow;` instead of including a GLFW header, keeping the GLFW type opaque in the public header while still exposing `getGlfwWindow()`. This is the right pattern where a public header needs to *name* a backend type without needing its full definition — use it the same way for any future public-facing pointer-to-backend-type.
- **What belongs in headers vs. `.cpp` files:** class declarations, public/protected/private member declarations, and small inline getters (e.g. `Window::getGlfwWindow()` is defined inline in the header) go in headers. All non-trivial logic — anything touching OpenGL/GLFW calls, file I/O, or more than a couple of lines — goes in the `.cpp` file. This is already followed consistently; keep it that way.

### Types

- **`unsigned int` for GPU handles**, not `GLuint` and not a fixed-width type — every OpenGL object ID in the codebase (`Shader::m_program`, `Texture::m_id`, `VertexBuffer::m_id`, `VertexArray::m_id`, `IndexBuffer::m_id`) is a plain `unsigned int`. This matches OpenGL's own `GLuint` definition and avoids an unnecessary GL-type dependency in headers that don't otherwise need `<glad/glad.h>`. Keep using `unsigned int` for handles in new code.
- **`int` for plain counts/sizes that are OpenGL-adjacent** (`Texture::m_width/m_height/m_channels`, `Window::m_sizeX/m_sizeY`) — matches `stbi_load`'s and GLFW's own `int`-based APIs. Keep this for anything that's a direct pass-through to/from those libraries.
- **`size_t` for byte counts / buffer sizes** (`VertexBuffer`, `IndexBuffer`, `Mesh` constructors all take `size_t size`) — keep using `size_t` for raw byte-size parameters.
- **Fixed-width integers (`uint32_t` etc.) are effectively unused** — there's exactly one occurrence in the whole codebase (`uint32_t index` in `VertexArray::addBuffer`). Don't introduce fixed-width integer types as a new pattern; stick with `unsigned int`/`int`/`size_t` as above unless you have a specific bit-width requirement (e.g. serialization, which doesn't exist yet).
- **`float` for per-frame values, `double` for accumulated/absolute time** — `Time::getDeltaTime()` returns `float` (precision loss over one frame is negligible), while `Time::sinceStart()`/`sinceStartOfFrame()`/`sinceLastFrame()` return `double` (avoids drift accumulating over a long-running session). This is a deliberate, sensible distinction already in the code — follow it for any new time- or accumulation-related values.
- **References for required, non-null dependencies** — `Material::Material(Shader &shader)`, `Material::setTexture(const std::string&, Texture &texture)`. If something must exist and can't be null, take it by reference.
- **Raw pointers for non-owning, optional-or-nullable access** — `Input::Input(Window *window)` (borrows, doesn't own), `Material::TextureSlot::texture` (borrowed from a caller-owned `Texture`). See §4 for the full ownership picture.
- **`const` on read-only methods** — `Shader`, `Texture`, `Camera`, `Transform`, `Mesh`, `VertexArray`/`VertexBuffer`/`IndexBuffer` all mark their read-only methods `const` (`bind() const`, `getViewMatrix() const`, `draw() const`, etc.). **Inconsistency:** `Window`'s getters (`getWidth()`, `getHeight()`, `getAspectRatio()`, `shouldWindowsClose()`) and all of `Input`'s query methods (`isKeyDown()`, `isMouseButtonDown()`, etc.) are **not** marked `const`, despite being logically read-only. **Standard for new code: mark every method `const` that doesn't mutate observable state.** Don't retroactively `const`-qualify `Window`/`Input` unless you're already touching those files for another reason — it's a non-breaking, low-risk change, but not worth a dedicated pass on its own.
- **`nullptr`** — used consistently wherever a null pointer literal appears in newer code (e.g. `Shader::readFile` returning `nullptr`). Some GLFW-facing calls still pass `NULL` (a few spots in `Window.cpp`, matching GLFW's own C-style API convention in its function signatures) — that's fine when directly interfacing with a C API; use `nullptr` everywhere else.
- **`auto`** — used sparingly and only where the type is already obvious from context: range-based for-loop variables (`for (const auto &tex : m_textures)` in `Material::bind()`) and `std::get<T>(variant)` results. Don't use `auto` to hide a type that isn't otherwise obvious at the call site.

---

## 4. Ownership and Lifetime

Neon3D has **no smart-pointer-heavy ownership model** today — ownership is mostly implicit and RAII-based at the single-object level, with exactly one `std::unique_ptr` in the whole codebase.

**Current state, concretely:**
- **`Window`** owns its GLFW handle via `std::unique_ptr<GLFWwindow, GLFWWindowDeleter>` with a custom deleter (`Window.hpp`) — this is the one place smart-pointer ownership is used, and it's the right pattern (RAII wrapper around a C-API handle with a custom destroy function).
- **`Input`** does **not** own the `Window` it's constructed with — it takes a raw, non-owning `Window *window` and extracts a raw `GLFWwindow*` from it (`m_window`) for its own use. The caller (`Sandbox/main.cpp`) is responsible for keeping the `Window` alive for at least as long as the `Input` object exists. This is intentional non-ownership, but it's currently undocumented in the header — a comment above `Input(Window *window)` noting "non-owning, caller must keep Window alive" would help.
- **`Shader`**, **`Texture`**, **`VertexBuffer`**, **`VertexArray`**, **`IndexBuffer`** each own their single OpenGL handle directly (an `unsigned int m_id`/`m_program`) and clean it up in their destructor (`glDeleteTextures`, `glDeleteBuffers`, `glDeleteVertexArrays`). This is straightforward RAII at the individual-resource level — no smart pointer needed since there's exactly one handle per object and no sharing.
- **`Mesh`** owns its `VertexBuffer`, `VertexArray`, and `IndexBuffer` as direct value members (not pointers) — full ownership by composition, cleaned up automatically via their own destructors.
- **`Material`** holds a **non-owning reference** to its `Shader` (`Shader &m_shader` — the `Shader` must outlive the `Material`) and **non-owning raw pointers** to its `Texture`s (`std::vector<TextureSlot>`, where `TextureSlot::texture` is a raw `Texture*` obtained from a reference parameter in `setTexture()`). The caller owns all `Texture`/`Shader` objects; `Material` just borrows them.
- **`Sandbox/main.cpp`'s `Cube` class manages `Texture` ownership manually with raw `new`/`delete`** (`m_texture = new Neon::Texture(...)` in the constructor, `delete m_texture` in the destructor) — this is inconsistent with the RAII pattern used everywhere in `Engine/`, and is the one place in the whole repository where manual `new`/`delete` appears. **This is technical debt, not a pattern to copy.** New Sandbox code (or any future engine code needing owned, heap-allocated resources) should use `std::unique_ptr` instead, matching `Window`'s own pattern.

**Rules for new code:**
- **Value members** when a class fully owns a small object with no need for polymorphism or optional-ness (`Mesh` owning its buffers — keep doing this).
- **`std::unique_ptr`** for owned, heap-allocated, single-owner resources — especially anything wrapping a C-API handle needing a custom destroy call, following `Window`'s pattern.
- **`std::shared_ptr`** — not used anywhere today. Don't introduce it unless a resource genuinely needs multiple independent owners (this will likely come up once a resource/asset manager exists — see §10 — but don't add it preemptively for single-owner cases).
- **`std::weak_ptr`** — not used anywhere today, and there's no current need for it.
- **Raw pointers** only for **non-owning** access (borrowing), never for ownership. If you write a raw pointer member, document in a comment (or eventually the header) that it's non-owning and who is expected to keep the pointee alive.
- **References** for required, always-valid, non-owning dependencies passed at construction (`Material`'s `Shader&` is the model to follow).
- **Avoid raw `new`/`delete` in new code entirely** — the one existing instance in `Sandbox/main.cpp`'s `Cube` is the counter-example, not the standard.

---

## 5. Error Handling

Neon3D distinguishes two kinds of failure, and the existing code already draws this line reasonably consistently — this section documents the existing intent, it doesn't invent a new system.

### `NEON_ASSERT` — programmer errors / violated invariants

Use when a condition being false means the *engine* or *calling code* did something it should never do if used correctly — not something a user or the environment could trigger through normal, legitimate use.

The macro (`Engine/include/.../Assert.hpp`, wherever it lives in your tree):

```cpp
#ifdef NEON_DEBUG
#define NEON_ASSERT(condition, message)    \
    do {                                    \
        if (!(condition)) {                 \
            Neon::Logging::Error(message);  \
            assert(condition);              \
        }                                   \
    } while (false)
#else
#define NEON_ASSERT(condition, message) \
    do { (void)sizeof(condition); } while (false)
#endif
```

**This only fires in builds where `NEON_DEBUG` is defined.** As of this writing, neither `Engine/CMakeLists.txt` nor `Sandbox/CMakeLists.txt` defines `NEON_DEBUG` under any configuration — so until that's wired up (e.g. via `target_compile_definitions(Engine PUBLIC $<$<CONFIG:Debug>:NEON_DEBUG>)`), `NEON_ASSERT` calls compile to a no-op everywhere. Be aware of this when adding new assertions: they won't do anything until the build defines `NEON_DEBUG` for Debug configs.

Because the Release branch evaluates the condition inside `sizeof(...)` (an unevaluated context), **the condition must be free of side effects** — anything inside a `NEON_ASSERT(...)` only actually executes in Debug builds, never in Release. Never write `NEON_ASSERT(++counter < limit, ...)`. The condition must be safe to evaluate exactly once (it's technically evaluated twice in the Debug branch — once in the `if`, once in `assert()` — so a side-effecting condition would behave differently between the two evaluations too).

**Concrete examples from Neon3D's actual code (not yet present, but representative of the invariant each system should protect):**
- `Shader::bind()` calling `glUseProgram(m_program)` when `m_program == 0` — the shader failed to compile/link, and `bind()` was called anyway. `NEON_ASSERT(m_program != 0, "Attempting to bind an invalid shader")`.
- `Mesh`'s constructor being given an `indexSize` that isn't a multiple of `sizeof(unsigned int)` — silently truncates the index count via integer division. `NEON_ASSERT(indexSize % sizeof(unsigned int) == 0, "Mesh index buffer size must be a multiple of sizeof(unsigned int)")`.
- `Material::set()`'s `if`/`else if` chain over `MaterialProperty`'s `std::variant` alternatives having no final `else` — if a new type is ever added to the variant without updating this function, it silently does nothing. A trailing `NEON_ASSERT(false, "Unhandled MaterialProperty type")` protects against that class of future mistake.

### `Logging::Error` — runtime/external failures

Use when the failure is a legitimate possibility during normal operation — a missing file, a bad external input, something the environment did, not something the calling code did wrong. This is already the established pattern:

- `Shader::compileShader()` — GLSL file not found, or fails to compile (`Shader.cpp`).
- `Shader::linkProgram()` — program link failure (`Shader.cpp`).
- `Texture::Texture(...)` — `stbi_load` returns `nullptr` (bad path, corrupt/unsupported image) (`Texture.cpp`).
- `Window::Window(...)` — `glfwInit()`, window creation, or `gladLoadGLLoader()` failing (`Window.cpp`) — genuine environment failures (missing drivers, no display, unsupported GL version), not programmer error.

### `Logging::Warning` — suspicious but not necessarily wrong

Not currently used anywhere in the engine — no existing call sites to draw from. When you do reach for it, the bar is: something that's *probably* a mistake but doesn't prevent the engine from continuing correctly. A reasonable future example given the current code: `VertexArray::addBuffer()` currently uses `Logging::Error` + early return when `getGlType()` doesn't recognize a `ShaderDataType` — that's arguably closer to a `Warning` case if the layout can safely skip that one attribute and continue, versus an `Error` if it aborts the whole buffer setup. Don't retroactively change this without deciding on the tradeoff first.

### `Logging::Info` — normal diagnostic information

Used consistently today for successful setup/lifecycle events: `"GLFW initialized"`, `"GLFW window created"`, `"Shader ... is compiled"`, `"Shaders linked"`, `"Texture loaded: ..."`, `"VBO created"`, `"VAO created"`. Keep using it the same way for new systems' setup/teardown milestones — not for per-frame or high-frequency events (nothing in the current codebase logs per-frame, and that should stay true; per-frame `Logging::Info` calls would flood the console).

---

## 6. Assertions

(See §5 for the mechanics of `NEON_ASSERT` and the build-flag caveat — this section is about *when* to reach for it.)

- **Do assert:** invalid object state before use (`m_id != 0` before binding a GPU resource), violated lifecycle ordering (once Neon3D has APIs with an explicit order requirement — none exist yet, since there's no `Renderer`/`Scene` with begin/submit/end semantics today), programmer-controlled invariants like buffer-size alignment (`Mesh`'s index-size-multiple-of-4 case above), and "this branch should be unreachable" guards in exhaustive `switch`/`if-else` chains over closed types (the `Material::set()` example above).
- **Don't assert:** anything that can legitimately fail due to external input, missing files, bad user-provided paths, or driver/environment issues — those go through `Logging::Error` (§5). Don't assert `stbi_load` succeeded, don't assert a shader file exists, don't assert GLFW initialized.
- **No side effects in conditions.** The Release-mode `sizeof(...)` trick means side effects inside `NEON_ASSERT(...)` simply won't run outside Debug builds — this isn't a style preference, it's a correctness requirement given how the macro is implemented.
- **Conditions must be safe to evaluate exactly once** (even though the Debug branch technically evaluates twice) — no mutation, no counters, no `++`/`--`, no calls with observable side effects.
- **Assertions are not a substitute for `Logging::Error`.** If a normal, non-buggy program could hit the condition (a missing asset, a malformed but externally-sourced file), it's error handling, not an assert — see §5 for the dividing line, and don't move existing `Logging::Error` call sites to `NEON_ASSERT` without re-checking which category they actually belong to.

---

## 7. Architecture Rules

Neon3D's current systems, and their actual dependency relationships (verified from `#include`s, not assumed):

```
Logging   — depends on nothing engine-specific (only <string>, <iostream>)
Time      — depends on nothing engine-specific (only <chrono>)
Camera    — depends only on GLM
Transform — depends only on GLM
Window    — depends on GLFW + GLAD (owns the GL context)
Input     — depends on Window (constructor takes Window*) + GLFW
Shader    — depends on GLAD/GLFW + Logging
Texture   — depends on GLAD + stb_image + Logging
Material  — depends on Shader + Texture (composition, no GL calls of its own)
Mesh      — depends on VertexBuffer + VertexArray + IndexBuffer + GLAD
VertexBuffer / VertexArray / IndexBuffer — depend on GLAD/GLFW + Logging
```

**Intended direction, per the project roadmap** (Core → Platform → Rendering → Resources → Scene → Gameplay → Tools/Editor) — this document doesn't restate the whole roadmap, just the architecture rule relevant to day-to-day contributions:

**Systems that must remain renderer/platform-agnostic — verified against the actual code:**
- **`Camera`** — ✅ already correctly agnostic. `Camera.hpp`/`Camera.cpp` include only GLM. No OpenGL or GLFW type appears anywhere in its interface or implementation. Keep it this way — `Camera` should never need to know it's being used with OpenGL specifically.
- **`Transform`** — ✅ already correctly agnostic, same reasoning as `Camera`.
- **`Time`** — ✅ already correctly agnostic. Built entirely on `<chrono>`; doesn't know GLFW exists, despite GLFW having its own timing functions it could have used instead. Keep it this way.
- **`Logging`** — ✅ already correctly agnostic. Pure `<string>`/`<iostream>`, no GLFW dependency.

**Where backend leakage currently exists (be honest about this, don't pretend it's fixed):**
- **`Window`** exposes `GLFWwindow *getGlfwWindow()` directly in its public header (`Window.hpp`). This means any code that includes `Window.hpp` and calls this method can reach raw GLFW state — the abstraction is not airtight. `Input`'s constructor is the one legitimate internal consumer of this today (`Input.cpp`: `m_window(window->getGlfwWindow())`). **Intended rule going forward:** don't add *new* public call sites that reach through `getGlfwWindow()` — if a new system needs GLFW access, that's a sign it should either go through `Window`/`Input`'s existing API, or that `Window`'s API needs to grow a proper abstraction for whatever's missing, not another consumer of the raw handle.
- **`Input.hpp`** stores `GLFWwindow *m_window` as a private member and includes `Window.hpp`, which pulls the GLFW dependency chain in transitively. This is existing, accepted debt (see the project roadmap for the planned fix) — not something to replicate in new systems, but not something this document is asking you to fix as a side effect of unrelated work either.

**Rule for all new code:** if you're writing a system that isn't inherently about windowing or GPU submission (math, timing, logic, future scene/gameplay code), it should not include `glad/glad.h`, `glfw/glfw3.h`, or any OpenGL type in its **public header**. OpenGL/GLFW calls belong in `.cpp` files (see §8) or in the systems whose entire job is talking to the backend (`Window`, `Shader`, `Texture`, `Mesh`/buffer classes).

---

## 8. OpenGL / GLFW Rules

**Current state:**
- OpenGL types (`GLenum`, `GLuint`, etc.) and calls are confined to `.cpp` files across the board — no public header exposes a raw `GLenum`/`GLuint` in its interface (handles are stored as `unsigned int`, which happens to be GL-compatible without requiring the caller to include GL headers). This part is already done well.
- **GLFW is the one exception**, via `Window.hpp`'s `getGlfwWindow()` returning `GLFWwindow*` (see §7). This is a known, accepted leak — not a hidden one.

**Rules for new code:**
- Never add a new public header that includes `glad/glad.h` or `glfw/glfw3.h` directly, unless that header's entire purpose is backend interfacing (in the spirit of `Window`/`Shader`/`Texture`).
- Never return a raw `GLFWwindow*`, `GLenum`, or other backend type from a new public API unless there's no reasonable alternative — and if you do, document clearly (in the header comment) that it's an escape hatch, the way `getGlfwWindow()`'s existence already implicitly is.
- Keep OpenGL calls inside `.cpp` files. This is already the norm and should stay that way even as new systems are added.
- If you're adding a system that legitimately needs both a public, backend-agnostic interface *and* an OpenGL-specific implementation (the `Shader`/`Texture`/`Mesh` pattern), follow that existing pattern: public header with no GL types, `.cpp` file doing the actual `gl*` calls.

---

## 9. Rendering Conventions

Derived from `Sandbox/shaders/shader.vert` / `shader.frag` and how `Material`/`Shader` are used in `Sandbox/main.cpp` — these are currently **implicit** conventions (nothing enforces them), documented here so future shaders/materials stay consistent with the existing one.

- **Vertex attribute naming:** `a_` prefix — `a_Position`, `a_Normal`, `a_TexCoord`, `a_Tangent` (`shader.vert`).
- **Varying (vertex→fragment) naming:** `v_` prefix — `v_Normal`, `v_Color`, `v_TexCoord`, `v_TBN`. **Inconsistency to flag:** `FragPos` (also a varying, passed from vertex to fragment shader) does **not** follow the `v_` convention used by its siblings in the same file. **Convention for new shaders: use `v_` for every varying, including position-like ones (`v_FragPos`).** Don't fix the existing shader files as a drive-by change — only when you're touching them for another reason.
- **Uniform naming:** `u_` prefix for ordinary uniforms — `u_Model`, `u_View`, `u_Projection`, `u_Color`, `u_ViewPos`, `u_Shininess`, `u_IsLightSource`.
- **Texture sampler uniform naming:** `T_` prefix, distinct from `u_` — `T_Color`, `T_Normal` (`shader.frag`), and this is matched on the C++ side too: `m_mat.setTexture("T_Color", *m_texture)` in `Sandbox/main.cpp`. This looks like a deliberate sub-convention (samplers get `T_` to be visually distinct from scalar/vector uniforms at a glance), not an accident — it's used consistently in both the shader and the calling code. **Follow it for new shaders:** `u_` for ordinary uniforms, `T_` for sampler uniforms.
- **Vertex layout convention:** the current cube mesh in `Sandbox/main.cpp` uses a 4-attribute interleaved layout — position (`vec3`), normal (`vec3`), UV (`vec2`), tangent (`vec3`) — matching `a_Position`/`a_Normal`/`a_TexCoord`/`a_Tangent` in `shader.vert`, passed to `Mesh`'s constructor as `{{3, Float}, {3, Float}, {2, Float}, {3, Float}}`. There's no enforced standard vertex format — each `Mesh` defines its own layout via the `BufferElement` list passed to its constructor — but if you're not deviating for a specific reason, matching this attribute order (position, normal, UV, tangent) keeps new meshes shader-compatible with the existing `shader.vert` without changes.
- **Texture slot assignment:** handled automatically by `Material::bind()` — it iterates `m_textures` and assigns slots sequentially starting at `0` (`slot++` each iteration), setting the matching sampler uniform. Don't manually assign texture slots elsewhere; always go through `Material::setTexture()` + `Material::bind()`.
- **Camera/view conventions:** `Camera::getViewMatrix()` applies rotation in yaw (Y) → pitch (X) → roll (Z) order, then translates by `-position` (`Camera.cpp`). `Transform::getMatrix()` applies translate → rotate X → rotate Y → rotate Z → scale (`Transform.cpp`). These orders are **not the same rotation order** between `Camera` and `Transform` (Camera: Y, X, Z; Transform: X, Y, Z) — that's worth being aware of if you're reasoning about rotation behavior across the two, though it's not necessarily wrong (a camera's "look around" rotation and an object's "orient in space" rotation are different use cases with different natural conventions) — just don't assume they're interchangeable.
- **Depth and culling:** `Window`'s constructor enables `GL_DEPTH_TEST`, `GL_CULL_FACE` (culling `GL_BACK`), and sets front-face winding to `GL_CCW` (`Window.cpp`). **This means every mesh's front-facing triangles must use counter-clockwise winding**, or they'll be culled and invisible — this applies to the existing cube in `Sandbox/main.cpp` and to any new mesh data.
- **Projection:** fixed near/far clip planes of `0.1`/`100.0` in `Camera::getProjectionMatrix()` — not currently configurable per-camera. If you need different clip planes, that's a `Camera` API gap, not something to work around per-call.

---

## 10. Resource Management

**Current state, honestly:** there is **no `ResourceManager`/`AssetManager`** of any kind. Every `Shader`/`Texture` is constructed directly by application code with a file path, owned individually, and not shared or cached even if two objects load the exact same file.

- **`Shader`** — RAII-correct: constructor compiles+links, no destructor is defined (meaning the underlying GL program is currently **never explicitly deleted** — `Shader` has no destructor and doesn't call `glDeleteProgram`). This is a real gap, not a documented design choice — flagging it here rather than pretending `Shader` cleans up after itself.
- **`Texture`** — RAII-correct: constructor loads via `stbi_load` and uploads to GL, destructor calls `glDeleteTextures`.
- **`Mesh`** — RAII-correct by composition: owns `VertexBuffer`/`VertexArray`/`IndexBuffer` as value members, each of which cleans up its own GL handle in its own destructor.
- **`Material`** — doesn't own any GPU resources directly; it's a non-owning aggregator over a borrowed `Shader&` and borrowed `Texture*`s (see §4). Nothing to clean up.

**What future resource-management code should avoid** (relevant now, even without a `ResourceManager` yet):
- **Duplicate ownership** — don't let two objects both believe they own the same GL handle and both try to delete it. Today this isn't a live bug (nothing shares handles), but it's the failure mode to design against once a cache/manager exists.
- **Manual OpenGL handle management outside the owning class** — if you need a new GL resource type, wrap it in its own RAII class the way `Texture`/`VertexBuffer`/etc. do; don't call `glGen*`/`glDelete*` from application-level code (Sandbox) or from an unrelated engine class.
- **Unnecessary copies of GPU-resource-owning objects** — `Shader`, `Texture`, `VertexBuffer`, `VertexArray`, `IndexBuffer`, and `Mesh` all currently have **no explicitly deleted or defined copy constructor/assignment operator**. That means the compiler-generated copy constructor is currently in effect for all of them, which for e.g. `Texture` would copy the `m_id` handle bitwise — two `Texture` objects would then both believe they own (and both try to delete) the same GL texture. **This is a real, currently-unguarded footgun.** Until it's addressed, avoid copying these objects (pass by reference, as the code already does everywhere) rather than relying on the default copy behavior.

---

## 11. CMake / Build Rules

**Current state, from the actual `CMakeLists.txt` files:**

- **`Engine`** is built as a static library (`add_library(Engine ...)`) from every `.cpp` in `Engine/src/`, including the `Mesh/` subfolder. New source files must be added explicitly to the `add_library(Engine ...)` source list in `Engine/CMakeLists.txt` — there's no glob pattern picking up new files automatically.
- **`Sandbox`** is built as an executable (`add_executable(Sandbox src/main.cpp)`) and links `Engine` via `target_link_libraries(Sandbox PRIVATE Engine)`.
- **Include directories:** `Engine/CMakeLists.txt` sets `include` and `include/Mesh` as `PUBLIC` (so `Sandbox` — and any consumer — can `#include "Shader.hpp"` etc. without a subpath), `lib/glfw/include` and `lib/stb` as `PRIVATE` (internal to `Engine`'s own implementation), and `lib/glm` as `PUBLIC` (so consumers can use GLM types directly, matching `Material.hpp`'s public use of `glm::vec3` etc.).
- **Linking:** `Engine` privately links the prebuilt `libglfw3.a`, the `GLAD` target (built from `lib/glad`'s own `CMakeLists.txt` via `add_subdirectory`), and the Windows system libraries `opengl32`, `gdi32`, `user32`, `shell32`.
- **`NEON_DEBUG` is not currently defined anywhere in either `CMakeLists.txt`.** This directly affects `NEON_ASSERT` (see §5/§6) — as things stand, no assertion anywhere in the engine will ever fire in any build configuration until this is added, e.g.:
  ```cmake
  target_compile_definitions(Engine PUBLIC $<$<CONFIG:Debug>:NEON_DEBUG>)
  ```
  This document isn't asking you to add that as a side effect of an unrelated change — just be aware of it if you're relying on `NEON_ASSERT` actually firing.

**Adding a new source file:** add it to the `add_library(Engine ...)` list in `Engine/CMakeLists.txt`, in the same relative location as its header (mirroring `include/`'s structure, as already done for the `Mesh/` subfolder).

**What not to do:**
- Don't add compiler flags to individual source files via `set_source_files_properties` or similar unless there's a specific, documented reason (nothing in the current build does this — don't introduce per-file special-casing without cause).
- Don't add new `PRIVATE`/`PUBLIC` include directories or link targets without checking whether an existing entry already covers it — the current include/link setup is fairly minimal and deliberate.
- Don't vendor a new third-party dependency without updating `Engine/lib/` and `Engine/CMakeLists.txt` consistently with how `glad`/`glfw`/`glm`/`stb` are already integrated (see §12).

---

## 12. Third-Party Dependencies

All current dependencies, all vendored under `Engine/lib/` — there is no package manager (no vcpkg/Conan/FetchContent) in use:

| Dependency | Purpose | Vendoring | Engine code should... |
|---|---|---|---|
| **GLFW** | Window creation, OpenGL context, input polling | Prebuilt static lib (`Engine/lib/glfw/libglfw3.a`) + headers | ...only be touched from `Window`/`Input`. Public headers should not expose GLFW types beyond `Window`'s existing `GLFWwindow*` escape hatch (§7). |
| **GLAD** | OpenGL function loading | Source, vendored with its own `CMakeLists.txt` (`Engine/lib/glad`), built as its own CMake target and linked into `Engine` | ...only be included from `.cpp` files that make direct `gl*` calls (`Window`, `Shader`, `Texture`, `Mesh`/buffer classes). |
| **GLM** | Vector/matrix math | Header-only (`Engine/lib/glm`) | ...be used freely, including in public headers — `Camera`, `Transform`, and `Material` (`MaterialProperty`) all expose `glm::` types publicly today. GLM is treated as a math primitive, not a "backend" to hide, and that's a reasonable existing choice. |
| **stb_image** (`stb_image.h`) | Image loading | Header-only, single-file (`Engine/lib/stb`) | ...stay confined to `Texture.cpp` (its only current consumer). Don't include `stb_image.h` from a public header. |

Don't add a new third-party dependency without vendoring it the same way (its own folder under `Engine/lib/`, integrated into `Engine/CMakeLists.txt` explicitly).

---

## 13. Adding a New Engine System

1. Decide which architectural layer it belongs to (§7) — and whether it needs to be renderer/platform-agnostic (like `Camera`/`Transform`/`Time`) or is inherently backend-facing (like `Window`/`Shader`/`Texture`).
2. Create the public header in `Engine/include/` (or a new subfolder if the system has multiple related headers, following the `Mesh/` pattern).
3. Create the implementation file in the matching location under `Engine/src/`.
4. If the system is meant to be backend-agnostic, don't include `glad/glad.h` or `glfw/glfw3.h` in the header — keep any backend calls (if genuinely needed) in the `.cpp` file only.
5. Add the new `.cpp` file explicitly to `add_library(Engine ...)` in `Engine/CMakeLists.txt` (§11).
6. Decide ownership up front (§4) — value member, `std::unique_ptr`, or non-owning reference/pointer — and document non-owning pointer parameters in a comment.
7. Decide what's a `NEON_ASSERT` (programmer-error invariant) versus a `Logging::Error`/`Warning` (runtime failure) for this system (§5/§6) before writing the logic, not after.
8. Add `Logging::Info` calls for meaningful setup/lifecycle events, matching the existing style (`"X created"`, `"X loaded: <path>"`) — not per-frame.
9. Exercise the new system through `Sandbox` — it's currently the only way anything in `Engine` gets tested (see §17).
10. If the system changes or extends the public API in a way other contributors (or future-you) would need to know about, update this document or the relevant section of it — don't let conventions drift silently.

---

## 14. Adding a New Class

Following the existing style — using a hypothetical `Framebuffer` as an example (not a real system, purely illustrative of the conventions above):

**`Engine/include/Framebuffer.hpp`**
```cpp
#ifndef NEON_FRAMEBUFFER
#define NEON_FRAMEBUFFER

namespace Neon
{
    class Framebuffer
    {
    public:
        Framebuffer(unsigned int width, unsigned int height);
        ~Framebuffer();

        void bind() const;
        static void unbind();

        unsigned int getWidth() const;
        unsigned int getHeight() const;

    private:
        unsigned int m_id{};
        unsigned int m_width{};
        unsigned int m_height{};
    };
}

#endif
```

**`Engine/src/Framebuffer.cpp`**
```cpp
#include "Framebuffer.hpp"
#include "Logging.hpp"

#include "glad/glad.h"

namespace Neon
{
    Framebuffer::Framebuffer(unsigned int width, unsigned int height)
        : m_width(width), m_height(height)
    {
        glGenFramebuffers(1, &m_id);
        // ... setup ...

        Logging::Info("Framebuffer created");
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_id);
    }

    void Framebuffer::bind() const
    {
        NEON_ASSERT(m_id != 0, "Attempting to bind an invalid framebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, m_id);
    }

    void Framebuffer::unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    unsigned int Framebuffer::getWidth() const { return m_width; }
    unsigned int Framebuffer::getHeight() const { return m_height; }
}
```

Notes on why this example follows the conventions above (not restating every rule — see the relevant sections): `NEON_<NAME>` include guard, `Neon` namespace, `PascalCase` class name, `camelCase` methods, `m_`-prefixed private members, `unsigned int` for the GL handle, `const` on read-only methods, own-header-first include order, `NEON_ASSERT` for the invalid-handle-use case (not `Logging::Error`, since binding an invalid framebuffer is a programmer error, not a runtime one), and a `Logging::Info` on successful construction matching the existing `"X created"` style.

---

## 15. Sandbox Rules

**Sandbox is a consumer/example of the Engine's public API — not a place to implement engine functionality.**

**What belongs in Sandbox:** application-specific logic (the `Cube` class, the main loop, hardcoded scene content, camera-fly-around input handling) and its own assets/shaders (`Sandbox/assets/`, `Sandbox/shaders/`).

**What belongs in Engine:** anything reusable across more than one hypothetical consumer — buffer/shader/texture/material wrappers, math utilities, windowing/input abstractions.

**Current state — where Sandbox already violates these rules (technical debt, not a pattern to extend):**
- `Sandbox/main.cpp`'s `Cube` class constructs its `Shader` with a **hardcoded absolute Windows path** (`"C:/Users/wukbg/programing/C++/Neon3D/Sandbox/shaders/shader.vert"`) rather than a path relative to the executable or working directory. This makes Sandbox non-portable even to another machine running the same OS. New Sandbox code shouldn't repeat this — use a relative path or a path resolved at runtime.
- `Cube` manages `Texture` ownership with manual `new`/`delete` rather than `std::unique_ptr` (see §4) — don't extend this pattern; use `std::unique_ptr` for new owned heap resources in Sandbox.
- Sandbox directly includes and depends on `Neon::Input`'s GLFW-backed key enum and calls `window->getAspectRatio()`/`window->shouldWindowsClose()` directly in a hand-rolled `while` loop — this is expected today (`Application`/event-system abstractions don't exist yet per §1), not a violation, but if/when those are added, Sandbox's `main.cpp` should migrate to use them rather than continuing to hand-roll the loop.

**Rule for new Sandbox code:** if you find yourself writing something in `Sandbox/main.cpp` that feels reusable (a second `Cube`-like helper class, a math utility, an input-handling helper), stop and ask whether it belongs in `Engine/` instead. Sandbox growing its own parallel mini-framework is a sign something should have moved up into `Engine`.

---

## 16. API Design Rules

- **Const correctness** — mark every method that doesn't mutate state as `const` (§3 covers where this is and isn't currently followed).
- **Avoid unnecessary copies** — pass `Shader&`/`Texture&` by reference (as `Material` already does), not by value; return `const&` from getters where the returned object is expensive to copy and the caller doesn't need ownership (though note none of the current getters return references to internal state — `Camera`/`Transform`'s public data members are accessed directly instead, which sidesteps this question entirely for those two classes).
- **Explicit ownership** — see §4. A new public API should make it obvious from its signature alone whether it takes/returns owned or borrowed data (value/`unique_ptr` vs. reference/raw pointer).
- **Avoid exposing implementation details** — the one current exception is `Window::getGlfwWindow()` (§7/§8); don't add more escape hatches like it without a specific need.
- **Avoid unnecessary getters/setters** — `Camera` and `Transform` expose `position`/`rotation`/(`scale`/`FOV`) as **public data members** rather than via getter/setter pairs, since they're simple value types being mutated directly and frequently (every frame, in `Sandbox/main.cpp`'s camera-fly controls). This is a deliberate, reasonable choice for simple POD-like state — follow it for similarly simple value-holding classes, but don't apply it to classes wrapping a GPU resource (`Shader`/`Texture`/etc.), where direct field access could bypass the class's actual invariants.
- **Avoid leaking third-party types** — with the one documented exception of GLFW via `Window` (§7), and the deliberate, accepted use of GLM types in public headers (`Camera`, `Transform`, `Material`'s `MaterialProperty`) — GLM is treated as a math primitive rather than a backend to hide, which is consistent with how the engine already uses it everywhere.
- **Keep APIs small and intentional** — `Shader`'s uniform-setter API (`setBool`/`setInt`/`setUInt`/`setFloat`/`setVec2`/`setVec3`/`setVec4`/`setMat3`/`setMat4`) is a reasonable model: one function per type it actually needs to support, no speculative overloads for types nothing currently uses.

---

## 17. Testing

**Current state, honestly: there are no tests anywhere in the repository.** `Sandbox` is currently the only thing exercising `Engine`'s code paths, and it does so by running the engine, not by asserting expected behavior.

**Intended direction for when tests are introduced:**
- Start with **pure-logic systems that don't require an OpenGL context** — `Transform` (matrix construction from position/rotation/scale) and `Camera` (view/projection matrix construction, `getForward()`/`getRight()`) are the most straightforward candidates, since they're pure GLM math with no GPU state. `Time` is also a reasonable early candidate (delta-time/elapsed-time arithmetic over a mocked or real clock).
- Systems requiring a live GL context (`Shader`, `Texture`, `Mesh`, the buffer classes, `Window`) will need a different testing strategy — likely some form of headless/offscreen context setup — and should be tackled after the pure-logic systems have coverage, not before.
- No specific testing framework is currently chosen. Don't introduce one as a side effect of an unrelated change; that's a decision to make deliberately when testing work actually starts.

---

## 18. Git / Commit Guidelines

Lightweight, appropriate for a solo/small project — not a corporate workflow:

- Write commit messages that describe **what changed and why**, in plain language — `"Fix Logging::Error threshold check"` rather than `"fix bug"`.
- Prefer smaller, focused commits over large ones that mix unrelated changes (e.g. don't bundle a `Texture` bug fix with unrelated `Input` refactoring in the same commit).
- If a commit fixes a specific, known issue (like the ones identified in this document or a prior audit), reference it briefly in the message.
- No enforced commit message format (no Conventional Commits requirement, no ticket-number prefix) — just be clear enough that `git log` is actually useful to future-you.

---

## 19. Pull Requests / Changes

Even for a primarily solo/small-team project, a quick self-review checklist before merging:

- [ ] Builds successfully (`Engine` and `Sandbox`, both configurations you can test)
- [ ] No unnecessary new dependency introduced (§12 — vendoring a new library is a deliberate decision, not a side effect)
- [ ] Any public API changes are intentional, not accidental (adding/removing/renaming a public method or header)
- [ ] No new backend leakage (§7/§8 — no new `GLFWwindow*`/`GLenum` etc. in a public header without a specific reason)
- [ ] `NEON_ASSERT` vs. `Logging::Error`/`Warning`/`Info` choices match §5/§6's programmer-error-vs-runtime-failure distinction
- [ ] `Engine/CMakeLists.txt` updated if new source files were added (§11)
- [ ] `Sandbox` still builds and runs against the changed `Engine` code
- [ ] This document updated if the change establishes, breaks, or changes a convention described here

---

## 20. Before You Commit — Checklist

- [ ] Builds in Debug
- [ ] Builds in Release
- [ ] No new compiler warnings introduced
- [ ] Ownership/lifetime of any new object is unambiguous (owner uses `unique_ptr`/value member; borrowers use reference/raw pointer, documented as non-owning)
- [ ] `NEON_ASSERT`/`Logging::Error`/`Logging::Warning`/`Logging::Info` used per §5/§6, not interchangeably
- [ ] No unnecessary OpenGL/GLFW type exposed in a public header (§7/§8)
- [ ] `Engine/CMakeLists.txt` updated for any new/removed source file
- [ ] `Sandbox` still runs correctly against the change
- [ ] Public API changes are reflected here if they establish a new convention or break an existing one

---

## Repository Findings Used

Concrete, verified findings from the actual codebase that this document is based on:

- **No root `CMakeLists.txt`** was present in the inspected snapshot — only `Engine/CMakeLists.txt` and `Sandbox/CMakeLists.txt`.
- **`NEON_DEBUG` is defined nowhere** in either `CMakeLists.txt` — `NEON_ASSERT` currently no-ops in every build configuration.
- **`GLFWwindow*` leaks through `Window::getGlfwWindow()`** in the public header, and `Input.hpp` stores a raw `GLFWwindow* m_window` — the one confirmed backend-leakage point in an otherwise clean public-header boundary.
- **`Camera`, `Transform`, `Time`, `Logging`** are all confirmed genuinely backend-agnostic — no GL/GLFW includes anywhere in their headers or implementations.
- **`enum` vs. `enum class` inconsistency:** `Input::Key`/`Input::MouseButton` are unscoped `enum`; `ShaderDataType`, `TextureFilter`, `TextureWrap`, `LoggingLevel` are `enum class`.
- **Enum value casing inconsistency:** `Input`'s enum values are `UPPER_SNAKE_CASE` (GLFW-style); every other enum's values are `PascalCase`.
- **Member-variable prefix inconsistency:** `Input`'s protected members `keyData`/`mouseStatus` lack the `m_` prefix used by every other non-public member in the codebase (including `Input`'s own private `m_window`).
- **Include-guard naming inconsistency:** `Input.hpp` uses `INPUT_HPP`; all 12 other headers use `NEON_<NAME>`.
- **Include-order inconsistency across `.cpp` files**, verified file-by-file: `Window.cpp` (own header, then engine header, then third-party), `Shader.cpp` (engine header before own header), `Input.cpp`/`Mesh.cpp` (third-party before own header), `Material.cpp` (std lib before own header), `VertexBuffer.cpp` (engine header before own header) — no single consistent pattern currently exists.
- **`const`-qualification inconsistency:** `Window`'s and `Input`'s read-only getter/query methods are not `const`-qualified, while equivalent methods in `Shader`, `Texture`, `Camera`, `Transform`, `Mesh`, and the buffer classes are.
- **`std::unique_ptr` is used exactly once** in the entire codebase (`Window::m_window`, with a custom `GLFWWindowDeleter`). No `std::shared_ptr` or `std::weak_ptr` usage exists anywhere.
- **Manual `new`/`delete` exists in exactly one place:** `Sandbox/main.cpp`'s `Cube` class, managing `Texture*` ownership — inconsistent with the RAII pattern used everywhere in `Engine/`.
- **No copy-constructor/assignment guards** on `Shader`, `Texture`, `VertexBuffer`, `VertexArray`, `IndexBuffer`, or `Mesh` — all rely on the compiler-generated (bitwise) copy, which would double-own/double-delete a GL handle if any of them were ever copied.
- **`Shader` has no destructor** — never calls `glDeleteProgram`, unlike every other GL-resource-owning class in the codebase.
- **Shader naming conventions confirmed from `Sandbox/shaders/shader.vert`/`shader.frag`:** `a_` for attributes, `v_` for varyings (with `FragPos` as the one exception that doesn't follow this), `u_` for ordinary uniforms, `T_` for sampler uniforms (matched on the C++ side in `Sandbox/main.cpp`'s `setTexture("T_Color", ...)` calls).
- **Rotation-order difference between `Camera::getViewMatrix()` (Y→X→Z) and `Transform::getMatrix()` (X→Y→Z)**, confirmed by reading both implementations directly.
- **`Window`'s constructor enables `GL_DEPTH_TEST`, `GL_CULL_FACE`/`GL_BACK`, and `GL_CCW` front-face winding** — meaning all mesh data (including the existing Sandbox cube) must use counter-clockwise front-facing winding.
- **`uint32_t` appears exactly once** in the whole codebase (`VertexArray::addBuffer`'s local `index` variable); everywhere else uses `unsigned int`/`int` for GL-adjacent values.
- **`Cube`'s shader path in `Sandbox/main.cpp` is a hardcoded absolute Windows path** (`C:/Users/wukbg/...`), confirmed by direct inspection — a portability problem, not a convention to follow.
- **No `ResourceManager`/`AssetManager` exists** — confirmed by the absence of any such file in `Engine/include`/`Engine/src`, and by every `Shader`/`Texture` in `Sandbox/main.cpp` being constructed individually with no caching.
- **No tests exist anywhere in the repository** — confirmed by the absence of any test-related file, framework reference, or test target in either `CMakeLists.txt`.
- **Third-party dependency vendoring confirmed by directory inspection:** GLFW as a prebuilt `.a` (Windows-only), GLAD as source with its own `CMakeLists.txt`, GLM and stb_image as header-only.
