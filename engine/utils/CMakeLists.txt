# X-Ray Fixes — utils (modding/compile tools + their support libraries).
# Mirrors the utils/*.vcxproj projects from engine.sln. Only processed when
# XRAY_BUILD_UTILS is ON. Requires the engine runtime targets (defined before
# this directory is added).
#
# Conventions (same as the runtime file): file(GLOB) per module + explicit
# exclusions matching <ExcludedFromBuild>, per-config defines via generator
# expressions, PCH only where the vcxproj uses it. Every util links xrCore
# (unused import libs are harmless; missing ones are fatal).

# ---------------------------------------------------------------- xrQSlim ---
# Mesh optimizer static lib (Garland-Heckbert), no PCH. Mixed has no MIXED
# macro in the vcxproj, so Debug and Mixed share defines.
file(GLOB XRQSLIM_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrQSlim/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrQSlim/src/*.h")
add_library(xrQSlim STATIC ${XRQSLIM_SRCS})
target_include_directories(xrQSlim PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_compile_definitions(xrQSlim PRIVATE
  WIN32 _WINDOWS XR_QSLIM_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrQSlim PRIVATE xray_engine_eh xrCore)
target_link_libraries(xrQSlim PRIVATE xray_engine_eh xrCore)

# --------------------------------------------------------------------- LWO ---
# LightWave object loader DLL, no PCH. The .sln maps Mixed to Release.
file(GLOB LWO_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/LWO/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/LWO/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/LWO/*.h")
# main.c is a leftover test harness, in no vcxproj config.
list(FILTER LWO_SRCS EXCLUDE REGEX "/main\\.c$")
add_library(LWO SHARED ${LWO_SRCS})
target_compile_definitions(LWO PRIVATE
  WIN32 _WINDOWS _USRDLL LWO_EXPORTS _CRT_SECURE_NO_WARNINGS _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
target_link_libraries(LWO PRIVATE xray_engine_eh_release xrCore odbc32 odbccp32)

# ------------------------------------------------------------------- ctool ---
# Console tool, no PCH, Unicode (the only Unicode utils project, like the
# vcxproj CharacterSet). The .sln maps Mixed to Release.
file(GLOB CTOOL_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/ctool/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ctool/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ctool/*.h")
add_executable(ctool ${CTOOL_SRCS})
target_include_directories(ctool PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport")
target_compile_definitions(ctool PRIVATE
  WIN32 _CONSOLE _CRT_SECURE_NO_WARNINGS _UNICODE UNICODE
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
target_link_libraries(ctool PRIVATE xray_engine_eh_release xrCore)

# -------------------------------------------------------- CompressionTest ---
# LZO round-trip test tool (console, no PCH). Compiles xrCore/Model.cpp plus
# its own copy of the LZO sources from ../xrCompress/lzo. The .sln maps
# Mixed to Debug.
file(GLOB COMPRESSIONTEST_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/CompressionTest/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/CompressionTest/*.h")
list(APPEND COMPRESSIONTEST_SRCS
  "${XRAY_ENGINE_DIR}/xrCore/Model.cpp")
file(GLOB COMPRESSIONTEST_LZO CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/lzo/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/lzo/*.h")
list(APPEND COMPRESSIONTEST_SRCS ${COMPRESSIONTEST_LZO})
add_executable(CompressionTest ${COMPRESSIONTEST_SRCS})
target_include_directories(CompressionTest PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport")
target_compile_definitions(CompressionTest PRIVATE
  WIN32 _CONSOLE _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(CompressionTest PRIVATE xray_engine_eh xrCore)

# ----------------------------------------------------- mp_screenshots_info ---
# MP screenshot inspector (console, PCH pch.h). screenshots_writer.cpp is
# excluded in every config; xr_dsa_signer.cpp is Debug-excluded (so it builds
# in Release only — there is no Mixed config in the vcxproj). The .sln maps
# Mixed to Debug.
file(GLOB MPSS_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_screenshots_info/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_screenshots_info/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_screenshots_info/*.h")
list(FILTER MPSS_SRCS EXCLUDE REGEX "/screenshots_writer\\.cpp$")
list(FILTER MPSS_SRCS EXCLUDE REGEX "/xr_dsa_signer\\.cpp$")
list(APPEND MPSS_SRCS
  "$<$<CONFIG:Release>:${CMAKE_CURRENT_SOURCE_DIR}/mp_screenshots_info/xr_dsa_signer.cpp>")
add_executable(mp_screenshots_info ${MPSS_SRCS})
target_include_directories(mp_screenshots_info PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(mp_screenshots_info PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_screenshots_info/pch.h")
target_compile_definitions(mp_screenshots_info PRIVATE
  WIN32 _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG>)
target_link_libraries(mp_screenshots_info PRIVATE
  xray_engine_eh crypto xrCore)

# ------------------------------------------------------ mp_configs_verifyer ---
# MP configs checker (console-subsystem default, PCH pch.h). Release defines
# are empty in the vcxproj. The .sln maps Mixed to Debug.
file(GLOB MPCV_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_configs_verifyer/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_configs_verifyer/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_configs_verifyer/*.h")
add_executable(mp_configs_verifyer ${MPCV_SRCS})
target_include_directories(mp_configs_verifyer PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(mp_configs_verifyer PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/mp_configs_verifyer/pch.h")
target_compile_definitions(mp_configs_verifyer PRIVATE
  _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:WIN32;DEBUG>)
target_link_libraries(mp_configs_verifyer PRIVATE
  xray_engine_eh crypto xrCore)

# ------------------------------------------------------------------- xrDXT ---
# DXT codec DLL (PCH stdafx.h). DXT.cpp and NormalMapGen.cpp are excluded:
# they call nvDXTcompress() from NVIDIA's nvDXTlib, and the only copy in the
# repo (nvDXTlibMTDLL.lib, VS2003-era CRT) cannot link against the modern CRT
# (35 unresolved std:: imports) — same failure in engine.sln. Everything else
# (software codec in Image_DXTC, tga, NVI_*) builds and links without it.
# Tape-out note: restoring full xrDXT needs a modern nvDXTlib import lib.
file(GLOB XRDXT_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrDXT/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrDXT/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrDXT/*.h")
list(FILTER XRDXT_SRCS EXCLUDE REGEX "/(nvdxt|DXT|NormalMapGen)\\.cpp$")
add_library(xrDXT SHARED ${XRDXT_SRCS})
set_target_properties(xrDXT PROPERTIES OUTPUT_NAME "DXT")
target_include_directories(xrDXT PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport")
target_precompile_headers(xrDXT PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrDXT/stdafx.h")
target_compile_definitions(xrDXT PRIVATE
  WIN32 _WINDOWS _USRDLL DXT_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:FORCE_NO_EXCEPTIONS;DEBUG>
  $<$<CONFIG:Mixed>:FORCE_NO_EXCEPTIONS;DEBUG;NDEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrDXT PRIVATE xrCore)

# ------------------------------------------ xrLC / xrLC_Light (skipped) ---
# These two are intentionally NOT built: xrLC_Light calls DXTCompress() from
# xrDXT/DXT.cpp, which needs nvDXTcompress() from NVIDIA's nvDXTlib — and the
# only copy in the repo (utils/xrDXT/nvDXTlibMTDLL.lib) targets the VS2003 CRT
# and cannot link against the modern CRT (35 unresolved std:: imports).
# xrLC in turn needs lc_global_data() from xrLC_Light. engine.sln fails
# identically. Everything else builds without them. Restoring them needs a
# modern nvDXTlib import lib exporting nvDXTcompress (then re-add the target
# blocks from git history and link it into xrDXT).
# NOTE: xrLC and xrLC_Light need NVIDIA nvDXTlib (see above); xrDO_Light
# and xrAI need editors/LevelEditor/Engine/communicate.h plus the builder
# types it defines (b_texture, b_material, ...), which exist nowhere in
# the repo or any public fork (the editor tree was dropped upstream).
# engine.sln fails all four identically. The target blocks were removed;
# see git history to restore them if the dependencies ever reappear.
message(STATUS "Skipping xrLC/xrLC_Light (need NVIDIA nvDXTlib) and "
                "xrDO_Light/xrAI (need the dropped editor tree)")

# ------------------------------------------------------------ xrSE_Factory ---
# Server-entities factory DLL (PCH stdafx.h). Compiles a subset of
# xrServerEntities (everything except LevelGameDef.cpp); script_stack_tracker
# is Release-excluded; xr_LuaAllocator.c is in no config. C sources and
# xrServerEntities objects build without PCH (same policy as xrGame).
file(GLOB XRSEFACTORY_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSE_Factory/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSE_Factory/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSE_Factory/*.h")
list(FILTER XRSEFACTORY_SRCS EXCLUDE REGEX "/xr_LuaAllocator\\.c$")
file(GLOB XRSEFACTORY_SCRIPT_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/../xrServerEntities/*.cpp")
list(FILTER XRSEFACTORY_SCRIPT_SRCS EXCLUDE REGEX "/(LevelGameDef|script_stack_tracker)\\.cpp$")
list(APPEND XRSEFACTORY_SCRIPT_SRCS
  "$<$<NOT:$<CONFIG:Release>>:${CMAKE_CURRENT_SOURCE_DIR}/../xrServerEntities/script_stack_tracker.cpp>"
  "${CMAKE_CURRENT_SOURCE_DIR}/../xrServerEntities/doug_lea_memory_allocator.c")
add_library(xrSE_Factory SHARED ${XRSEFACTORY_SRCS} ${XRSEFACTORY_SCRIPT_SRCS})
target_include_directories(xrSE_Factory PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSE_Factory"
  "${CMAKE_CURRENT_SOURCE_DIR}/../xrServerEntities"
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrSE_Factory PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSE_Factory/stdafx.h")
set_source_files_properties(${XRSEFACTORY_SCRIPT_SRCS} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrSE_Factory PRIVATE
  WIN32 _WINDOWS _USRDLL XRSE_FACTORY_EXPORTS MSVC _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG>
  $<$<CONFIG:Mixed>:DEBUG;MIXED;dSINGLE>
  $<$<CONFIG:Release>:NDEBUG;LUABIND_NO_ERROR_CHECKING>)
target_link_libraries(xrSE_Factory PRIVATE
  xray_engine_eh luabind luajit xrCore)

# ------------------------------------------------------------------ ETools ---
# Editor-support static helpers, built as DLL (PCH stdafx.h).
# optimize_vertex_order.cpp is excluded in every config. Links the vorbis
# codec libs by target (the vcxproj names a vorbisenc_static.lib that no
# vcxproj builds; this build provides it).
file(GLOB ETOOLS_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/ETools/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/ETools/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/ETools/*.h")
list(FILTER ETOOLS_SRCS EXCLUDE REGEX "/optimize_vertex_order\\.cpp$")
add_library(ETools SHARED ${ETOOLS_SRCS})
# NOTE: dx/ is not in the vcxproj includes; MSBuild finds d3dx9.h via a
# machine-global DXSDK that clean machines lack. The repo vendors it.
target_include_directories(ETools PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}"
  "${XRAY_SDK_INCLUDE_DIR}/dx"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrQSlim/src")
target_precompile_headers(ETools PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/ETools/stdafx.h")
target_compile_definitions(ETools PRIVATE
  WIN32 _WINDOWS ETOOLS_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG;_USRDLL>
  $<$<CONFIG:Mixed>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(ETools PRIVATE
  xray_engine_eh xrCDB xrCore xrQSlim ogg vorbis vorbisenc vorbisfile)

# --------------------------------------------------------------- xrCompress ---
# LTX/archive compressor (console exe, PCH stdafx.h). Bundles its own LZO copy
# (C sources build without PCH).
file(GLOB XRCOMPRESS_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/*.h")
file(GLOB XRCOMPRESS_LZO CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/lzo/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/lzo/*.h")
add_executable(xrCompress ${XRCOMPRESS_SRCS} ${XRCOMPRESS_LZO})
target_include_directories(xrCompress PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrCompress PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCompress/stdafx.h")
set_source_files_properties(${XRCOMPRESS_LZO} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrCompress PRIVATE
  WIN32 _CONSOLE _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG>
  $<$<CONFIG:Mixed>:DEBUG;MIXED>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrCompress PRIVATE xray_engine_eh xrCore)

# Same AdditionalLibraryDirectories mirror as the runtime: resolves any
# `#pragma comment(lib, ...)` bare names the tools rely on.
foreach(_t IN ITEMS
    xrQSlim LWO ctool CompressionTest mp_screenshots_info mp_configs_verifyer
    xrDXT xrSE_Factory ETools xrCompress)
  if(TARGET ${_t})
    target_link_directories(${_t} PRIVATE
      "${CMAKE_BINARY_DIR}/lib/$<CONFIG>" "${XRAY_SDK_LIB_DIR}")
  endif()
endforeach()
