# Agents.md — C++ Cross‑Platform Game Engine (ECS, OpenGL, GLAD, CMake, Unit Tests)

## Mission
Build a cross‑platform (Windows, Linux, macOS) game engine in C++ with a custom ECS, OpenGL rendering (Core profile), GLAD for function loading, and fully automated builds via CMake. All third‑party libraries must be configurable and downloaded automatically (FetchContent). Provide unit tests for critical subsystems.

## Tech Stack (mandatory)
- Language: C++20 across all targets.
- Build: CMake ≥ 3.21.
- Graphics: OpenGL 4.5 Core profile (optionally 3.3 Core via `ENGINE_GL_MIN=ON`).
- GL function loader: GLAD (generated for the selected profile, static build).
- Windowing/Context/Input: GLFW (or SDL2 via option; default is GLFW).
- Math: GLM.
- Image loading: stb_image.
- Unit tests: Catch2 (or GoogleTest via option).
- Logging: lightweight built‑in logger; optional spdlog integration.
- Tooling: clang-format, clang-tidy (optional), Sanitizers (ASan/UBSan on Linux/macOS; MSVC sanitizer when available).

## High‑Level Architecture
```
engine/
  core/        // base types, time, UUID, logging
  ecs/         // Entity, Component, System, World, Queries
  resource/    // assets, loaders, caches
  render/      // renderer, shader, buffer, texture, mesh, camera
  platform/    // window, input, timers, file system
  math/        // glm wrappers, helpers
  runtime/     // main loop, app layer, minimal scene graph
  debug/       // stats, (optional) imgui integration
tests/
samples/
tools/
cmake/
```

### ECS Requirements
1. **Entity** — dense `uint32_t`/`uint64_t` IDs plus generation; no virtual dispatch.
2. **Components** — plain structs. Names must end with `Component`. No heavy resource ownership; use handles/descriptors.
3. **Storage** — archetype/chunk‑based is preferred; at minimum SoA + sparse set. Must support fast iteration over component sets.
4. **Systems** — plain functions or light types without virtuals. Registered via a system table and executed by a frame `Schedule`.
5. **One‑Frame Components** — event/impulse markers automatically cleared at the end of a frame.
6. **Queries** — compile‑time filters (templates) plus runtime masks for debugging.
7. **Synchronization** — single‑threaded initially; expose an interface that can be upgraded to a job system later.
8. **Determinism** — iteration order must be deterministic unless explicit sorting is requested.
9. **Serialization** — JSON for scenes in Stage 1; binary format deferred to a later milestone.

### Rendering (OpenGL)
- Core profile only; use VAO/VBO/IBO, UBO, SSBO where appropriate.
- Shaders: GLSL; compile at runtime, retain/emit compile logs.
- Materials: MVP is unlit + textured + vertex color; PBR is not required for Stage 1.
- Camera: perspective/orthographic via GLM matrices.
- Built‑in primitive meshes: quad, cube, plane; minimal `.obj` loader.
- Enable GL error validation in Debug (KHR_debug, `glDebugMessageCallback`).

### Platform
- GLFW for window/context/input. Abstract as `IWindow`, `IInput`.
- Time: high‑precision timer (std::chrono) with delta smoothing.
- File system: cross‑platform wrapper on top of `std::filesystem`.

### Resources
- `AssetManager` with hash‑based cache by path.
- Loaders: textures (stb_image), shaders (from files), models (minimal OBJ).

### Logging
- Built‑in minimal logger; enable `ENGINE_WITH_SPDLOG` to switch to spdlog.

## Build & CMake Requirements
- Single top‑level `CMakeLists.txt`.
- All third‑party libraries via `FetchContent`. No git submodules.
- Config flags for each dependency: `USE_GLFW`/`USE_SDL2`, `USE_CATCH2`/`USE_GTEST`, `ENGINE_WITH_IMGUI`, `ENGINE_WITH_SPDLOG`.
- Correct usage of interface targets and PRIVATE/PUBLIC linkage.
- Warning flags:
  - MSVC: `/W4 /permissive- /Zc:__cplusplus /EHsc`
  - GCC/Clang: `-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion`
- Sanitizer options: `ENGINE_ENABLE_ASAN`, `ENGINE_ENABLE_UBSAN` (disabled in Release).
- Provide `CMakePresets.json` for `debug`, `release`, `relwithdebinfo`, `asan`.
- Generate `compile_commands.json` for tooling support.

### CMake Options (example)
```cmake
option(USE_GLFW "Use GLFW for windowing" ON)
option(USE_SDL2 "Use SDL2 instead of GLFW" OFF)
option(USE_CATCH2 "Use Catch2 for tests" ON)
option(USE_GTEST "Use GoogleTest instead of Catch2" OFF)
option(ENGINE_WITH_IMGUI "Enable Dear ImGui integration" OFF)
option(ENGINE_WITH_SPDLOG "Enable spdlog logging" OFF)
option(ENGINE_ENABLE_ASAN "Enable AddressSanitizer (non-MSVC)" OFF)
option(ENGINE_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer (non-MSVC)" OFF)
option(ENGINE_GL_MIN "Target minimum GL 3.3 Core (instead of 4.5)" OFF)
```

### FetchContent (sample)
```cmake
include(FetchContent)

# GLFW
if(USE_GLFW)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
  )
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(glfw)
endif()

# GLAD (OpenGL loader)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v2.0.6
)
set(GLAD_PROFILE "core" CACHE STRING "")
set(GLAD_API "gl=4.5" CACHE STRING "")
FetchContent_MakeAvailable(glad)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.1
)
FetchContent_MakeAvailable(glm)

# stb (header-only)
FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(stb)

# Catch2 (or GoogleTest)
if(USE_CATCH2)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.4
  )
  FetchContent_MakeAvailable(catch2)
elseif(USE_GTEST)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
  )
  set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)
endif()
```

### Primary Targets
- `engine_core` (STATIC): core, ecs, resource, math, platform (interfaces), logging.
- `engine_render_gl` (STATIC): OpenGL renderer; links GLAD and GLM.
- `engine` (INTERFACE): convenience aggregates of includes/defines for consumers.
- `engine_runtime` (STATIC): game loop, `App`, minimal scenes.
- `engine_tests` (EXECUTABLE): unit test runner.
- `sandbox` (EXECUTABLE): sample app with triangle + textured quad.

### Linking Example
```cmake
add_library(engine_core STATIC ${ENGINE_CORE_SOURCES})
target_include_directories(engine_core PUBLIC include)
target_compile_features(engine_core PUBLIC cxx_std_20)

add_library(engine_render_gl STATIC ${ENGINE_RENDER_SOURCES})
target_link_libraries(engine_render_gl PRIVATE engine_core glad::glad glm::glm)
if(USE_GLFW)
  target_link_libraries(engine_render_gl PRIVATE glfw)
endif()

add_executable(sandbox ${SANDBOX_SOURCES})
target_link_libraries(sandbox PRIVATE engine_core engine_render_gl)
```

## Testing Strategy
- Default framework: Catch2.
- Cover: ECS (entity create/destroy, add/remove components, queries), One‑Frame events, sorted/deterministic iteration, resource loaders (mock FS), logger, math utilities.
- Target name: `engine_tests`. Run with `ctest`.
- Policy: any change in ECS/Render requires tests. Minimum line coverage 60% (non‑blocking but tracked).

### Test Template (example)
```cpp
#include <catch2/catch_test_macros.hpp>
#include "ecs/World.hpp"
#include "components/TransformComponent.hpp"

TEST_CASE("Add/Remove components") {
  ecs::World w;
  auto e = w.createEntity();
  REQUIRE_FALSE(w.has<TransformComponent>(e));
  w.add<TransformComponent>(e, {});
  REQUIRE(w.has<TransformComponent>(e));
  w.remove<TransformComponent>(e);
  REQUIRE_FALSE(w.has<TransformComponent>(e));
}
```

## Coding Standards
- Naming:
  - Components: `TransformComponent`, `CameraComponent`, `RenderableComponent`.
  - Systems: `TransformSystem`, `RenderSystem`.
  - Files: `CamelCase.cpp/.hpp`.
  - Namespaces: `engine::`, `ecs::`.
- No virtuals in hot paths. Virtual interfaces are allowed only at platform boundaries.
- Use `noexcept` where possible; apply `[[nodiscard]]` for significant return values.
- No exceptions on the render path. Prefer `expected<T, Error>` for recoverable errors.
- Enforce RAII for GL resources (wrappers for Buffer/Texture/Shader/Program).
- Public APIs documented in Doxygen style.
- Mandatory clang-format; `.clang-format` lives at repo root.

## Runtime Loop & Scheduling
1. Update time (variable timestep by default, with clamped max delta; fixed step optional).
2. Input → input systems → logic systems → render systems.
3. Clear One‑Frame components after `Render` step.
4. Event bus is modeled via One‑Frame components (e.g., `DamageEventComponent`).

## Milestones (with acceptance criteria)
### M0 — Bootstrap (1–2 days)
- CMake skeleton, presets, FetchContent wiring (GLFW/GLAD/GLM/stb/Catch2).
- Targets `engine_core`, `engine_render_gl`, `sandbox`, `engine_tests` exist.
- `sandbox` renders a colored triangle. `ctest` runs smoke tests.
**Acceptance:** builds on Windows (MSVC) and Linux (GCC/Clang).

### M1 — ECS MVP (3–5 days)
- Entity/Component/World, sparse set, add/remove APIs, iteration.
- One‑Frame components with auto cleanup.
- Unit tests covering all operations.
**Acceptance:** 1,000 iterations over 10k entities < 50 ms in Release on a mid‑range CPU (guideline).

### M2 — Render Core (3–5 days)
- RAII GL wrappers, shader compilation, texture loading, basic material.
- `sandbox`: textured quad and a moving camera.
- Tests: shader compile (mock), texture loader (fake FS).

### M3 — Resources (2–3 days)
- AssetManager + caching; OBJ loader (positions/uv/normals).
- Tests: cache hits/misses, proper unload semantics.

### M4 — Debug & Validation (2 days)
- GL KHR_debug, simple overlay of frame stats (without ImGui).
- Tests: logger stability, overlay toggles.

### M5 — Polish (2 days)
- Public API docs, examples, improved presets, optional tidy profile.
- Tag `v0.1.0` release.

## Non‑Functional Requirements
- Portability: identical CMake UX on all three OSes.
- Performance: no allocations on hot paths; reserve containers ahead of time.
- Reproducibility: dependencies pinned to tags/versions; allow overrides via CMake variables.
- Zero manual prep: `cmake -S . -B build -D<options>` downloads and builds everything.

## Directory Layout (strict)
```
/CMakeLists.txt
/CMakePresets.json
/cmake/   # helper scripts
/engine/
  include/engine/...
  src/core/...
  src/ecs/...
  src/resource/...
  src/render/gl/...
  src/platform/...
  src/runtime/...
/samples/sandbox/ (main.cpp, assets/)
/tests/
/thirdparty/ (empty; populated by FetchContent)
/tools/
```

## Example: Public Headers (skeleton only)
```cpp
// ecs/entity.hpp
#pragma once
#include <cstdint>
namespace ecs {
  using EntityHandle = uint32_t;
  struct Entity { EntityHandle id; uint32_t gen; };
}

// ecs/world.hpp
#pragma once
#include "entity.hpp"
namespace ecs {
  class World {
  public:
    Entity createEntity();
    void destroyEntity(Entity e) noexcept;
    template<class T, class... Args> T& add(Entity e, Args&&...);
    template<class T> bool has(Entity e) const noexcept;
    template<class... Ts, class Fn> void each(Fn&& fn);
  };
}
```

## Example: Sandbox Main
```cpp
int main() {
  engine::Runtime app;
  app.init();
  while (app.tick()) {
    // update systems
  }
  return 0;
}
```

## Continuous Integration (recommended)
- GitHub Actions matrix: `windows-latest`, `ubuntu-latest`.
- Build + tests; cache CMake and FetchContent.
- Artifacts: sandbox binaries for all three platforms.

## Definition of Done
- `sandbox` builds and runs on at least two OSes.
- `ctest` and basic perf checks pass locally.
- Setup and run instructions in `README.md` plus public API comments.
- All dependencies are auto‑fetched and configurable via CMake options.
- Code is clang‑formatted and passes static checks in Debug (tidy optional).

## Developer Commands (minimum)
```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DUSE_GLFW=ON -DUSE_CATCH2=ON
cmake --build build -j
ctest --test-dir build --output-on-failure
./build/samples/sandbox/sandbox   # run the sample
```
