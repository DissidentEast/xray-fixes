# Shared X-Ray CMake settings. Included once from the top-level CMakeLists.txt.
# Scope: game runtime only, Win32 (32-bit), VS 2022 (v143), C++17.

# --- Platform / toolchain guard ------------------------------------------------
# The legacy codebase is Win32-only (all vcxproj files target Win32 + v143 + MBCS).
if(NOT MSVC)
  message(WARNING "X-Ray Fixes CMake is tested with MSVC (Visual Studio 2022). "
                  "Other compilers are not supported by this starter.")
endif()
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR
    "X-Ray Fixes is a 32-bit (Win32) codebase. "
    "Re-configure with a Win32 generator, e.g. "
    "cmake -S . -B build -G \"Visual Studio 17 2022\" -A Win32")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 90)

# xrCore.h mandates exceptions ON in Debug/Mixed and OFF in Release, so the
# default /EHsc cannot stay global: CMake's VS generator maps it to an
# <ExceptionHandling> element that would override any per-target /EHsc-.
# Strip it here; engine targets get per-config flags via xray_engine_eh
# (engine/CMakeLists.txt), 3rd-party/SDK targets get plain /EHsc below.
string(REPLACE "/EHsc" "" _xray_cxx_flags "${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${_xray_cxx_flags}" CACHE STRING "C++ flags" FORCE)
unset(_xray_cxx_flags)

# VS toolset + Windows SDK come from the generator invocation
# (-G "Visual Studio 17 2022" -A Win32); the vcxproj files use v143 + 10.0,
# so any VS2022 + Win10-SDK setup matches.

# Multi-config setups (Visual Studio): Debug / Mixed / Release.
# "Mixed" is a legacy X-Ray config: debug info + light optimization.
# CMake knows nothing about it, so seed its flags from the Debug flags.
set(CMAKE_CONFIGURATION_TYPES "Debug;Mixed;Release" CACHE STRING "" FORCE)
if(MSVC)
  foreach(lang C CXX)
    if(NOT CMAKE_${lang}_FLAGS_MIXED)
      set(CMAKE_${lang}_FLAGS_MIXED "${CMAKE_${lang}_FLAGS_DEBUG}"
        CACHE STRING "Flags for Mixed config" FORCE)
    endif()
  endforeach()
  foreach(kind EXE MODULE SHARED STATIC)
    if(NOT CMAKE_${kind}_LINKER_FLAGS_MIXED)
      set(CMAKE_${kind}_LINKER_FLAGS_MIXED "${CMAKE_${kind}_LINKER_FLAGS_DEBUG}"
        CACHE STRING "Linker flags for Mixed config" FORCE)
    endif()
  endforeach()
endif()

# Shared CRT: MultiThreadedDLL (MD) in ALL configs — this matches every
# engine/*.vcxproj (even Debug uses MD, not MDd). The few exceptions override
# this per-target in engine/CMakeLists.txt: libogg/libvorbis (static CRT) and
# libtheora/oalib (MDd in Debug).
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")

# LTCG (/GL + /LTCG) in Release only — matches the vcxproj files that set
# WholeProgramOptimization for Release. NOTE: this is applied explicitly per
# target (xray_release_ltcg in engine/CMakeLists.txt), NOT via CMake's
# INTERPROCEDURAL_OPTIMIZATION: with the VS generator CMake enables /GL at
# compile time but does not reliably emit the matching /LTCG link flag, which
# produces LNK2001 (IL-only objects linked without codegen).
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE FALSE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MIXED FALSE)

# Out-of-source friendly output layout (bin/<Config>, lib/<Config>).
# The legacy .sln wrote to <repo>/build/<Config>/{binaries,libraries}; when you
# configure with `-B build` the layout below is equivalent in practice.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
foreach(cfg DEBUG MIXED RELEASE)
  string(TOLOWER "${cfg}" _lc)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg} "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg} "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
  unset(_lc)
endforeach()

# Common repo locations.
set(XRAY_ROOT_DIR "${CMAKE_SOURCE_DIR}")
set(XRAY_ENGINE_DIR "${XRAY_ROOT_DIR}/engine")
set(XRAY_SDK_DIR "${XRAY_ROOT_DIR}/SDK")
set(XRAY_SDK_INCLUDE_DIR "${XRAY_SDK_DIR}/include")
set(XRAY_SDK_LIB_DIR "${XRAY_SDK_DIR}/libraries")

# Global MSVC comfort flags matching the vcxproj defaults:
#  - /MP parallel builds (xrGame already used MultiProcessorCompilation)
#  - /fp:fast (all engine modules set FloatingPointModel=Fast)
#  - large-address-aware binaries (all engine Link blocks set it)
if(MSVC)
  add_compile_options(/MP /fp:fast)
  add_link_options(/LARGEADDRESSAWARE)
endif()
