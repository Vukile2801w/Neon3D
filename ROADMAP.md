# Neon3D — Development Roadmap

This roadmap reflects the **actual current state** of the Neon3D codebase (C++17, CMake, OpenGL 4.6 Core, GLFW, GLAD, GLM, Windows/MinGW), based on direct inspection of `Engine/include`, `Engine/src`, and the `Sandbox` example. It is not a generic engine-tutorial roadmap — tasks already implemented are marked done, and phases are ordered around Neon3D's real dependency graph.

---

# Architecture Direction

Neon3D today is a **thin OpenGL/GLFW wrapper**, not yet a layered engine. The long-term architecture should separate into:

| Layer | Responsibility | May depend on |
|---|---|---|
| **Core** | Logging, Time, assertions, config, containers | nothing engine-specific |
| **Platform** | Window, Input, OS/windowing backend (currently GLFW) | Core |
| **Rendering** | RenderAPI abstraction, Shader, Texture, Material, Mesh/buffers, Camera | Core, Platform (indirectly, via window handle only) |
| **Resources** | Asset manager, loading, caching, hot reload | Core, Rendering (for GPU resource creation) |
| **Scene** | Entities/components, Transform, hierarchy, serialization | Core, Resources |
| **Physics** | Collision, rigid bodies (future) | Core, Scene |
| **Audio** | Sound playback (future) | Core, Resources |
| **Gameplay** | Game objects, scripting hooks, state machines (future) | Scene, Physics, Audio |
| **Tools/Editor** | Debug UI, inspector, viewport (future) | all of the above |

**Systems that must NOT depend directly on OpenGL or GLFW types in their public headers:** `Camera`, `Transform`, `Time`, `Logging`, any future `Scene`/ECS/Physics/Audio/Gameplay code. These are pure logic/data and must stay renderer-agnostic.

**Systems that legitimately need OpenGL, but should hide it behind an interface eventually:** `Shader`, `Texture`, `Mesh`/`VertexBuffer`/`VertexArray`/`IndexBuffer`. Today they call OpenGL directly with no `RenderAPI` interface in between — acceptable for a single-backend solo project short-term, but flagged as debt (see below).

**Systems that currently leak the backend where they shouldn't:** `Window` forward-declares and exposes `GLFWwindow*` via `getGlfwWindow()`, and `Input` stores a raw `GLFWwindow*` internally. This is the main architectural crack to close in Phase 1.

---

## Phase 0 — Foundation
**Goal:** Solid project scaffolding, conventions, and diagnostics to build everything else on.
**Why it matters:** Cheap to fix now, expensive to retrofit once more systems depend on it.
**Dependencies:** none.

- [x] CMake-based `Engine` (static lib) / `Sandbox` (consumer executable) split
- [x] Third-party libs vendored under `Engine/lib` (GLFW, GLAD, GLM, stb_image)
- [x] Basic static logger (`Logging::Info/Warning/Error`, level filtering, ANSI color)
- [x] Frame timing utility (`Time`: delta time, elapsed time)
- [x] Add an assertion macro (`NEON_ASSERT`) that logs + breaks/aborts in debug builds
  - [x] Distinguish debug-only asserts from always-on runtime checks
- [x] Add a build-config header (`NEON_DEBUG` / `NEON_RELEASE` macros driven by CMake)
- [x] Add basic engine-wide error/result type (or exceptions policy) — decide once, document it, use it consistently (currently mixed: some failures just log and continue, e.g. `Window` constructor)
- [ ] Write a `CONTRIBUTING.md` / coding-conventions doc (naming, ownership rules, header style) — needed because current code mixes styles (`m_` prefix in `Window`/`Camera` but not consistently elsewhere)
- [ ] Set up a second compiler target (Linux/GCC or Clang) even if not fully working yet, to start surfacing portability issues early

**Definition of Done:** New contributors (including future-you) can build, get readable logs, and see clear pass/fail from assertions — without reading source to understand conventions.

---

## Phase 1 — Core Engine
**Goal:** Turn today's "manual main-loop + raw GLFW handle" pattern into a real application/platform layer.
**Why it matters:** This is Neon3D's biggest architectural gap right now — `Window` and `Input` work, but leak GLFW types and have no event system, so every consumer (like `Sandbox`) hand-rolls its own main loop.
**Dependencies:** Phase 0.

- [x] `Window`: creates GLFW window + OpenGL 4.6 core context, loads GLAD, enables depth test + back-face culling
- [x] `Window`: framebuffer resize callback keeps width/height/aspect ratio in sync
- [x] `Input`: polling-based keyboard + mouse button state (down/pressed/released)
- [ ] **Remove `GLFWwindow*` from `Window`'s public API.** Replace `getGlfwWindow()` with an opaque native-handle accessor used only internally by `Input`, not exposed to application code
- [ ] **Give `Input` its own ownership of platform callbacks instead of borrowing `Window`'s raw handle** — reduce coupling so `Input` depends on an abstraction, not a concrete GLFW pointer
- [ ] Wire up mouse scroll (`scroll_callback` in `Input.cpp` is currently a stub / TODO — `getMouseScrollValue()` always returns 0)
- [ ] Add mouse cursor mode control (normal / hidden / disabled) for FPS-style camera look, exposed through `Window` or `Input`, not raw GLFW calls
- [ ] Design a minimal event system (e.g. `WindowResizeEvent`, `KeyEvent`, `MouseMoveEvent`) with a simple dispatcher/callback registration — needed before Phase 2's renderer needs to react to resizes, and before any UI/editor work in Phase 8
- [ ] Introduce an `Application` class that owns `Window` + `Input` + `Time` and runs the main loop, so `Sandbox/main.cpp` stops hand-rolling `while (!window.shouldWindowsClose())` boilerplate
  - [ ] `Application::run()` drives `beginFrame()` / `handleInput()` / user update callback / `render()`
  - [ ] Expose an overridable `onUpdate(float dt)` hook for consumer code
- [ ] Multi-window support is explicitly **out of scope** for now (not a near-term priority — flag as "nice to have" only)

**Definition of Done:** A new Sandbox-like consumer can subclass/configure `Application` and never touch `GLFWwindow*`, GLFW headers, or hand-write a main loop.

---

## Phase 2 — Rendering
**Goal:** Formalize rendering into a `Renderer` layer instead of application code calling `material.bind()` / `mesh.draw()` directly in a loop.
**Why it matters:** Today all "rendering" logic lives in `Sandbox/main.cpp`. There is no `Renderer` class, no render-command submission, no render passes — meshes are drawn immediately and synchronously wherever `draw()` is called.
**Dependencies:** Phase 1 (needs `Window`/`Application` context, ideally the event system for resize-driven viewport/projection updates).

- [x] `Shader`: compiles/links vertex+fragment GLSL, built-in fallback shaders, full uniform setter API (`setBool/Int/UInt/Float/Vec2/Vec3/Vec4/Mat3/Mat4`)
- [x] `Texture`: `stb_image`-backed 2D texture loading, filter/wrap configuration, bind to slot
- [x] `Material`: binds a `Shader` + named textures + `MaterialProperty` variant uniforms, auto-assigns texture slots
- [x] `Mesh` + `VertexBuffer` + `VertexArray` + `IndexBuffer`: interleaved vertex layout via `BufferElement`/`ShaderDataType`, indexed draw
- [x] `Camera`: view/projection matrices, FOV, forward/right vectors (pure GLM, correctly decoupled from OpenGL)
- [x] `Transform`: position/rotation/scale → model matrix (correctly decoupled)
- [x] Depth testing and back-face culling enabled at window/context creation
- [ ] Introduce a `Renderer` class that owns the frame lifecycle: `beginScene(camera)` → `submit(mesh, material, transform)` → `endScene()`
  - [ ] Decide now whether submission is immediate-mode (call `draw()` directly, simplest) or queued (collect draw calls, sort, then flush) — **recommend starting immediate-mode**, revisit only if profiling shows a need
- [ ] Move camera/view/projection uniform-setting out of `Sandbox/main.cpp` and into `Renderer::beginScene`, so materials don't each need `set("u_View", ...)` boilerplate
- [ ] Add a basic `RenderAPI`-style seam even if OpenGL is the only backend for now — i.e., keep raw `gl*` calls confined to `.cpp` files (already mostly true) and never leak `GLenum`/GL types into public headers (already mostly true — verify and lock this in with a lint/review pass)
- [ ] Framebuffer/render-target abstraction (needed later for post-processing, shadow maps, editor viewport) — **not urgent yet**, defer until Phase 9 unless Phase 8 (editor viewport) needs it sooner
- [ ] Basic lighting is currently hardcoded per-shader in `Sandbox/shaders/shader.frag` (e.g. the recent back-face lighting-skip fix) — extract common lighting uniform names/conventions (`u_ViewPos`, light structs) into a documented Material/Shader convention so future shaders are consistent
- [ ] Render-pass concept (opaque pass, transparent pass, etc.) — defer to Phase 9, not needed for a single-mesh sandbox today

**Definition of Done:** Sandbox code calls `renderer.submit(...)` per object instead of manually binding materials, setting camera uniforms, and calling `mesh.draw()` itself.

---

## Phase 3 — Scene System
**Goal:** Move from "one hardcoded mesh + one hardcoded transform in `main.cpp`" to a real scene of multiple objects.
**Why it matters:** Nothing exists here yet — no entity concept, no hierarchy, no scene container. This blocks any real game content.
**Dependencies:** Phase 2 (Renderer must exist to iterate a scene and submit draw calls).

- [ ] Decide ECS vs. simple scene-graph vs. hybrid — **recommendation: start with a simple `GameObject { Transform, Mesh*, Material* }` list**, not a full ECS. A full ECS is over-engineering at this project size; revisit only if object count/perf demands it
- [ ] `Scene` class: owns a list of objects, exposes `addObject`/`removeObject`, iterates for update + render submission
- [ ] Optional parent/child transform hierarchy — **note:** `Transform::getMatrix()` currently has no parent concept at all; adding hierarchy means either a `Transform* parent` pointer or moving hierarchy into `Scene`/`GameObject` instead of into `Transform` itself (keep `Transform` a pure local-space math object)
- [ ] Basic scene serialization (start with a simple custom text/JSON format — do not build a generic reflection system yet)
- [ ] Scene loading/unloading lifecycle hooks

**Definition of Done:** Sandbox can load a scene file describing multiple objects with meshes/materials/transforms, and the engine renders all of them without per-object code in `main.cpp`.

---

## Phase 4 — Asset System
**Goal:** Replace "construct `Texture`/`Shader` by hand with a raw path every time" with managed, cached resources.
**Why it matters:** Right now every `Texture`/`Shader`/`Material` is manually constructed and owned by application code (see `Sandbox/main.cpp`) — no caching, no dedup, no hot reload, no lifetime management beyond RAII on individual objects.
**Dependencies:** Phase 2 (needs `Shader`/`Texture`/`Mesh` to exist, which they do) — can proceed in parallel with Phase 3.

- [x] `Texture` and `Shader` are already RAII-correct individually (own their GL handles, clean up in destructors)
- [ ] `ResourceManager`/`AssetManager`: load-by-path with caching so the same texture/shader isn't loaded twice
- [ ] Reference counting or handle-based access (`Ref<Texture>` style) instead of raw ownership by application code
- [ ] Model/mesh loading from a real file format (currently `Mesh` only takes raw vertex/index arrays hardcoded in `Sandbox/main.cpp` — no `.obj`/`.gltf` loader exists at all)
- [ ] Shader hot-reload (watch file, recompile on change) — quality-of-life, not a blocker for anything else
- [ ] Texture/asset caching keyed by path + load parameters (filter/wrap settings)

**Definition of Done:** Loading the same texture or shader twice from two different objects returns a shared resource, and a basic 3D model file can be loaded into a `Mesh` without hand-writing vertex arrays.

---

## Phase 5 — Physics
**Goal:** Basic collision + rigid body support.
**Why it matters:** Not started at all. Deliberately placed after Scene/Assets — physics without a scene to attach to is premature.
**Dependencies:** Phase 3 (Scene), Phase 1 (Time, for fixed timestep).

- [ ] Decide: integrate a third-party library (Jolt, Bullet, Box2D-for-2D) vs. hand-rolled — **recommendation: integrate an existing library**; writing a physics engine is its own multi-year project and not the goal here
- [ ] Physics abstraction layer so `Scene`/gameplay code doesn't depend on the chosen library's types directly
- [ ] Fixed-timestep update loop (decouple physics step from variable render frame time — `Time` currently only exposes variable delta time)
- [ ] Collision shapes, rigid bodies, triggers, layers/masks (only once a library is chosen — scope depends entirely on that choice)

**Definition of Done:** A cube can fall under gravity and rest on a plane in the Sandbox scene.

---

## Phase 6 — Audio
**Goal:** Basic sound playback.
**Why it matters:** Not started. Lower priority than physics for a "usable engine" milestone.
**Dependencies:** Phase 4 (Asset system, for loading sound files).

- [ ] Choose backend (miniaudio is a good low-friction fit for a solo C++ project)
- [ ] Audio abstraction: `AudioClip` (asset), `AudioSource` (playback instance)
- [ ] 2D sound effects and music playback
- [ ] 3D positional audio (defer until Scene has real spatial objects worth panning around)

**Definition of Done:** A sound effect can be triggered from Sandbox code and music can loop in the background.

---

## Phase 7 — Gameplay Framework
**Goal:** Give application code a structured way to write actual game logic instead of editing engine `main.cpp` files.
**Why it matters:** Not started. Currently "gameplay" is just inline code in `Sandbox/src/main.cpp` (camera fly-controls, hardcoded cube).
**Dependencies:** Phase 3 (Scene).

- [ ] Component/behavior hook pattern (e.g. `onUpdate`, `onStart`) attachable to scene objects — keep this simple; do not build a scripting language/VM unless there's a concrete need
- [ ] Timer/scheduler utility (delayed calls, repeating calls) built on `Time`
- [ ] Basic finite state machine helper (reusable, not gameplay-specific)
- [ ] Input action mapping layer on top of `Input` (e.g. "Jump" bound to `KEY_SPACE`, so gameplay code isn't hardcoding raw keys — though hardcoding raw keys, as Sandbox does today, is a legitimate "should have later, not now" call)

**Definition of Done:** A simple gameplay behavior (e.g. object that moves/reacts on a timer) can be written without touching engine internals.

---

## Phase 8 — Tools / Editor
**Goal:** Basic debug UI and inspection tools.
**Why it matters:** Not started. Lower priority than a working runtime — but debug UI (even just an FPS counter / stat overlay) has outsized value-per-effort and can be pulled earlier if useful.
**Dependencies:** Phase 2 (Renderer), Phase 3 (Scene, for an inspector to have something to inspect).

- [ ] Integrate Dear ImGui for debug overlays (frame time, entity list, basic value tweaking) — this is the highest-value, lowest-effort item in this phase and could reasonably be pulled forward once Phase 2's Renderer exists
- [ ] Simple scene inspector (list objects, edit transform values live)
- [ ] Asset browser (list loaded/available assets)
- [ ] Gizmos for transform manipulation in the viewport — defer, meaningful effort for limited payoff until there's an actual editor viewport
- [ ] Full standalone scene editor — **explicitly "nice to have," not required for a "usable engine"**

**Definition of Done:** Frame stats and a live-editable object list are visible while Sandbox runs, without recompiling.

---

## Phase 9 — Advanced Rendering
**Goal:** Move past "one directional light in a fragment shader" toward a modern forward/deferred pipeline.
**Why it matters:** Not started, and deliberately last among rendering work — flashy rendering features are the most common trap for solo engine projects to over-invest in before the foundation is solid.
**Dependencies:** Phase 2 (Renderer must be a real abstraction first, not ad hoc draw calls).

- [ ] Shadow mapping (currently no shadows at all — lighting is unshadowed per-fragment in `shader.frag`)
- [ ] HDR + tonemapping
- [ ] Post-processing pass infrastructure (needs the framebuffer abstraction deferred from Phase 2)
- [ ] PBR material model (current `Material`/shader is a basic Phong-style setup based on the Sandbox shader)
- [ ] SSAO, GPU instancing, compute shaders — **nice to have, only after everything above is solid**

**Definition of Done:** At least one advanced technique (recommend shadow mapping first) is integrated behind the existing `Material`/`Renderer` abstractions without special-casing application code.

---

## Phase 10 — Production Readiness
**Goal:** Make the engine safe to actually ship something with.
**Why it matters:** Not started. Comes last by design — polish and process matter far less than architecture until there's something substantial to polish.
**Dependencies:** all prior phases, informally.

- [ ] Unit tests for pure-logic systems (`Transform`, `Camera`, `Time` are the easiest first candidates — no GL context needed to test them)
- [ ] CI build (at minimum: build on push, even without tests initially)
- [ ] Cross-platform build verification (Linux at minimum, given GLFW/GLAD/GLM are all already cross-platform-capable)
- [ ] Profiling hooks (frame time breakdown by system)
- [ ] Packaging/release build configuration (strip debug logging overhead, asset packaging)
- [ ] Example projects beyond Sandbox (a second, different sample game) to validate the public API is actually usable by "someone who isn't you"

**Definition of Done:** A second, independent sample project can be built against the engine using only public headers/docs, with CI passing.

---

# Milestones

### M0 — Foundation
- **Objective:** Solid diagnostics and conventions.
- **Expected capabilities:** Assertions, consistent error handling, documented conventions.
- **Major systems:** Logging, build config.
- **Exit criteria:** Phase 0 checklist complete.

### M1 — Core Engine
- **Objective:** Clean platform layer with no leaked backend types.
- **Expected capabilities:** `Application` drives the loop; `Window`/`Input` hide GLFW entirely; basic event system exists.
- **Major systems:** Window, Input, Application, Events.
- **Exit criteria:** `Sandbox/main.cpp` no longer includes GLFW or writes its own `while` loop.

### M2 — Rendering
- **Objective:** Real `Renderer` abstraction.
- **Expected capabilities:** Scene submission API; camera/view/projection handled once, centrally.
- **Major systems:** Renderer, Shader, Material, Texture, Mesh, Camera.
- **Exit criteria:** Application code never manually sets `u_View`/`u_Projection` uniforms itself.

### M3 — Scene
- **Objective:** Multi-object scenes.
- **Expected capabilities:** Load/iterate/render a scene of several objects, not one hardcoded cube.
- **Major systems:** Scene, GameObject, Transform hierarchy.
- **Exit criteria:** Sandbox loads a scene description instead of hardcoding one mesh.

### M4 — Assets
- **Objective:** Managed, cached resources.
- **Expected capabilities:** Shared texture/shader loading, real model loading.
- **Major systems:** ResourceManager.
- **Exit criteria:** Duplicate asset loads are deduplicated; a real model file loads into a `Mesh`.

### M5 — Physics
- **Objective:** Basic dynamics.
- **Major systems:** Physics library integration.
- **Exit criteria:** An object falls, collides, and rests correctly in a scene.

### M6 — Audio
- **Objective:** Basic sound.
- **Major systems:** Audio backend integration.
- **Exit criteria:** SFX + music playback both work from gameplay code.

### M7 — Gameplay
- **Objective:** Structured game logic.
- **Major systems:** Behavior/component hooks, timers.
- **Exit criteria:** A gameplay behavior exists with zero engine-internal edits.

### M8 — Editor
- **Objective:** Basic live debug tooling.
- **Major systems:** ImGui integration, inspector.
- **Exit criteria:** Live stats + object inspection while running.

### M9 — Advanced Rendering
- **Objective:** Modern rendering techniques.
- **Major systems:** Shadow mapping, PBR, post-processing.
- **Exit criteria:** At least shadow mapping shipped through existing abstractions.

### M10 — Production Ready
- **Objective:** Shippable, testable, cross-platform.
- **Major systems:** CI, tests, packaging.
- **Exit criteria:** A second sample project builds against the engine via CI.

---

# Current Status

| System | Status | Notes |
|---|---|---|
| CMake Engine/Sandbox structure | ✅ Complete | Clean static-lib split already in place |
| Logging | ✅ Complete | Recently fixed: `Warning` spelling, `Error` level threshold bug |
| Time | ✅ Complete | Chrono-based delta/elapsed time |
| Camera | ✅ Complete | Pure GLM, correctly decoupled from rendering backend |
| Transform | ✅ Complete | Pure GLM, no hierarchy (by design, so far) |
| Shader | 🟡 In Progress | Full uniform API works; no hot-reload; no backend abstraction |
| Texture | 🟡 In Progress | Loading/binding/filtering work; no asset caching |
| Material | 🟡 In Progress | Solid variant-based uniform dispatch; no asset-manager integration |
| Mesh / VertexArray / VertexBuffer / IndexBuffer | 🟡 In Progress | Functional indexed rendering; no model-file loading, immediate-draw only |
| Window | ⚠️ Needs Refactor | Works, but leaks `GLFWwindow*` via `getGlfwWindow()` |
| Input | ⚠️ Needs Refactor | Works for keyboard/buttons, but stores raw `GLFWwindow*`; scroll is a stub |
| Application/main-loop abstraction | 🔴 Not Started | Loop is hand-written in `Sandbox/main.cpp` |
| Event system | 🔴 Not Started | Resize handled internally in `Window` only, not exposed generically |
| Renderer abstraction | 🔴 Not Started | Draw calls issued directly from application code |
| Scene system | 🔴 Not Started | One hardcoded object in Sandbox |
| Asset manager | 🔴 Not Started | Resources constructed and owned ad hoc |
| Physics | 🔴 Not Started | — |
| Audio | 🔴 Not Started | — |
| Gameplay framework | 🔴 Not Started | — |
| Tools/Editor | 🔴 Not Started | — |
| Advanced rendering (shadows/PBR/post-fx) | 🔴 Not Started | Current lighting is a single hardcoded Phong-style fragment shader |
| Tests/CI | 🔴 Not Started | — |

---

# Technical Debt

**Critical**
- `Window::getGlfwWindow()` exposes `GLFWwindow*` publicly, and `Input` stores a raw `GLFWwindow*` — the two systems most likely to need replacing/porting are the two currently most tightly coupled to GLFW. Fix before building `Application`/event system on top (Phase 1), or the coupling will spread further.

**High**
- No `Renderer` abstraction — every draw call is issued ad hoc from `Sandbox/main.cpp` (`material.bind()` + manual uniform sets + `mesh.draw()`). Any second consumer project will duplicate this boilerplate. Fix in Phase 2 before Scene work (Phase 3) makes it worse by multiplying the copy-pasted pattern per object.
- Mouse scroll input is declared in the public API (`getMouseScrollValue()`) but not implemented (`scroll_callback` is a stub) — a silently-broken public API surface is worse than a missing one. Fix or remove the declaration until implemented.

**Medium**
- No asset caching/deduplication — loading the same texture twice creates two independent GPU resources. Not urgent while there's one Sandbox scene, but will waste GPU memory as soon as Phase 3 introduces multiple objects sharing textures.
- Inconsistent error handling: some failures (e.g. GLFW/GLAD init failure in `Window`) log and continue rather than throwing/returning a result — decide on a single policy in Phase 0 rather than letting the pattern diverge further.
- Lighting/uniform naming conventions (`u_ViewPos`, etc.) live implicitly in `Sandbox/shaders/shader.frag` with no documented contract between `Material`/`Shader` and shader authors.

**Low**
- `Transform` has no parent/hierarchy concept — fine today, but decide *where* hierarchy lives (Transform vs. Scene/GameObject) before Phase 3, rather than bolting it on reactively.
- Minor naming inconsistencies across the codebase (e.g. `m_` prefix used in some classes, not others) — cosmetic, worth a pass once conventions are documented in Phase 0.

---

# Priority Order

1. **Phase 0** — assertions, config, conventions (cheap now, expensive later)
2. **Phase 1** — fix the `Window`/`Input` GLFW-leak, add `Application` + event system
3. **Phase 2** — introduce a real `Renderer` abstraction
4. **Phase 4** — basic asset manager (can run in parallel with Phase 3; unblocks real content)
5. **Phase 3** — Scene system (multi-object rendering)
6. **Phase 7** — minimal gameplay hooks (only once Scene exists)
7. **Phase 8 (partial)** — pull ImGui debug overlay forward here if it helps development velocity
8. **Phase 5** — physics
9. **Phase 6** — audio
10. **Phase 9** — advanced rendering
11. **Phase 10** — production readiness (ongoing in parallel with everything once Phase 1–2 are stable, not a single late phase in practice — start CI as early as Phase 1 if feasible)

This order optimizes for **architecture → core abstractions → renderer foundation → resource management → scene → gameplay**, deliberately deferring flashy rendering (Phase 9) and full tooling (Phase 8's heavier items) until the foundation won't need to be reworked underneath them.

---

# Immediate Next Steps

Concrete, actionable tasks based on the exact current state of the repo:

1. Remove `Window::getGlfwWindow()` from the public interface (or make it internal-only/friend-restricted); have `Input` obtain what it needs without exposing `GLFWwindow*` to application code.
2. Implement `scroll_callback` in `Input.cpp` so `getMouseScrollValue()` actually works, or temporarily remove the method from the public header until it's implemented.
3. Write an `Application` class (`Engine/include/Application.hpp` + `.cpp`) that owns `Window`, `Input`, `Time`, and runs the main loop; port `Sandbox/main.cpp`'s `while (!window->shouldWindowsClose())` loop into it.
4. Add a `NEON_ASSERT(condition, message)` macro in a new `Core/Assert.hpp`, and use it in `Window`'s constructor instead of just logging on GLFW/GLAD init failure.
5. Design and implement a minimal `Renderer` class with `beginScene(Camera&)` / `submit(Mesh&, Material&, Transform&)` / `endScene()`, and move the per-frame `material.set("u_View", ...)` / `u_Projection` calls out of `Sandbox/main.cpp` into it.
6. Add a basic `WindowResizeEvent` + simple callback-registration mechanism so `Renderer`/future systems can react to resizes without polling `Window::getWidth()/getHeight()` manually every frame.
7. Start a `ResourceManager` stub: even a simple `std::unordered_map<path, shared_ptr<Texture>>` cache is enough to eliminate duplicate texture loads before Scene work begins.
8. Write a `CONTRIBUTING.md` documenting naming conventions (settle the `m_` prefix inconsistency) and the error-handling policy (log-and-continue vs. assert vs. exceptions) before more systems are added.
9. Add a minimal model-loading path (start with a simple `.obj` loader) so `Mesh` can be built from a real asset file instead of hardcoded vertex arrays in `Sandbox/main.cpp`.
10. Stand up a bare-bones CI workflow (build-only, no tests yet) so build breakage is caught immediately going forward — cheapest time to add this is now, before the codebase grows.
