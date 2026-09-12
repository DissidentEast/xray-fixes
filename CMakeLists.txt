cmake_minimum_required(VERSION 3.20)

# X-Ray Fixes (S.T.A.L.K.E.R. Clear Sky) — CMake build, game runtime only.
# Coexists with the existing engine/engine.sln + *.vcxproj (those are untouched).
#
# Usage (VS 2022, 32-bit — the engine is Win32-only):
#   cmake -S . -B build -G "Visual Studio 17 2022" -A Win32
#   cmake --build build --config Release
# See CMakePresets.json for ready-made presets.

project(XRayFixes LANGUAGES C CXX)

include(cmake/xray.cmake)

option(XRAY_DEDICATED_SERVER "Build xrEngine as dedicated server (adds DEDICATED_SERVER)" OFF)
option(XRAY_BUILD_RENDER_R1 "Build xrRender_R1 (DX9)" ON)
option(XRAY_BUILD_RENDER_R2 "Build xrRender_R2 (DX9)" ON)
option(XRAY_BUILD_RENDER_R3 "Build xrRender_R3 (DX10)" ON)
option(XRAY_BUILD_UTILS "Build utils/ modding tools (xrAI, xrLC, ...)" ON)

add_subdirectory(engine)
