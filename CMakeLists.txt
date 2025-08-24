cmake_minimum_required(VERSION 3.21)
project(EcsAiEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

option(USE_GLFW "Use GLFW for windowing" ON)
option(USE_SDL2 "Use SDL2 instead of GLFW" OFF)
option(USE_CATCH2 "Use Catch2 for tests" ON)
option(USE_GTEST "Use GoogleTest instead of Catch2" OFF)
option(ENGINE_WITH_IMGUI "Enable Dear ImGui integration" OFF)
option(ENGINE_WITH_SPDLOG "Enable spdlog logging" OFF)
option(ENGINE_ENABLE_ASAN "Enable AddressSanitizer (non-MSVC)" OFF)
option(ENGINE_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer (non-MSVC)" OFF)
option(ENGINE_GL_MIN "Target minimum GL 3.3 Core (instead of 4.5)" OFF)

if(MSVC)
  add_compile_options(/W4 /permissive- /Zc:__cplusplus /EHsc)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion)
endif()

if(USE_GLFW)
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
  )
  set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
  set(GLFW_BUILD_X11 ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(glfw)
endif()

FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v2.0.6
)
FetchContent_MakeAvailable(glad)
add_subdirectory(${glad_SOURCE_DIR}/cmake ${glad_BINARY_DIR})
if(ENGINE_GL_MIN)
  set(GLAD_API gl:core=3.3)
else()
  set(GLAD_API gl:core=4.5)
endif()
glad_add_library(glad STATIC API ${GLAD_API})

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG f58f558c120e9b32c217290b80bad1a0729fbb2c
)
FetchContent_MakeAvailable(stb)

if(USE_CATCH2)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
  )
  FetchContent_MakeAvailable(catch2)
endif()

add_library(engine_core
  engine/src/runtime/runtime.cpp
  engine/src/ecs/world.cpp
)
target_include_directories(engine_core PUBLIC engine/include)
target_link_libraries(engine_core PUBLIC glm glad glfw)

  add_library(engine_render_gl
    engine/src/render/gl/shader.cpp
    engine/src/render/gl/buffer.cpp
    engine/src/render/gl/vertex_array.cpp
  )
target_include_directories(engine_render_gl PUBLIC engine/include)
target_link_libraries(engine_render_gl PUBLIC glad)

add_executable(sandbox samples/sandbox/main.cpp)
target_link_libraries(sandbox PRIVATE engine_core engine_render_gl)

if(USE_CATCH2)
  add_executable(engine_tests tests/test_dummy.cpp tests/test_ecs.cpp)
  target_link_libraries(engine_tests PRIVATE engine_core Catch2::Catch2WithMain)
  include(CTest)
  add_test(NAME engine_tests COMMAND engine_tests)
endif()
