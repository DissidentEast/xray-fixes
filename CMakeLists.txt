cmake_minimum_required(VERSION 3.20)

# X-Ray Fixes (S.T.A.L.K.E.R. Clear Sky) — CMake build, game runtime only.
# Coexists with the existing engine/engine.sln + *.vcxproj (those are untouched).
#
# Usage (VS 2022):
#   cmake -S . -B build     -G "Visual Studio 17 2022" -A Win32   (32-bit, as shipped)
#   cmake -S . -B build-x64 -G "Visual Studio 17 2022" -A x64     (64-bit port)
#   cmake --build build --config Release
# See CMakePresets.json for ready-made presets.

project(XRayFixes LANGUAGES C CXX)

include(cmake/xray.cmake)

option(XRAY_DEDICATED_SERVER "Build xrEngine as dedicated server (adds DEDICATED_SERVER)" OFF)
option(XRAY_BUILD_RENDER_R1 "Build xrRender_R1 (DX9)" ON)
option(XRAY_BUILD_RENDER_R2 "Build xrRender_R2 (DX9)" ON)
option(XRAY_BUILD_RENDER_R3 "Build xrRender_R3 (DX10)" ON)
# The utils/ tools are Win32-only (MFC front-ends + 32-bit-era lzo asm); the
# game never loads them, so there is no need for arch-matched tool binaries.
include(CMakeDependentOption)
cmake_dependent_option(XRAY_BUILD_UTILS "Build utils/ modding tools (xrAI, xrLC, ...)" ON
  "NOT XRAY_64BIT" OFF)

add_subdirectory(engine)
