# X-Ray Fixes — game-runtime CMake targets (Win32, VS2022/v143, C++17).
# Mirrors engine/engine.sln projects needed to produce the shipping binaries
# in bin/ (xrEngine.exe + runtime DLLs). Utils, editors and other tools from
# the .sln are intentionally out of scope.
#
# Source handling: file(GLOB) per module + explicit exclusions matching the
# vcxproj <ExcludedFromBuild> entries. Per-config defines use generator
# expressions ($<CONFIG:Debug> / $<CONFIG:Mixed> / $<CONFIG:Release>).

# ---------------------------------------------------------------- 3rd party ---
# zlib (static) — vcxproj excludes example.c, no PCH.
file(GLOB ZLIB_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/zlib/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/zlib/*.h")
list(FILTER ZLIB_SRCS EXCLUDE REGEX "example\\.c$")
add_library(zlib STATIC ${ZLIB_SRCS})
target_compile_definitions(zlib PRIVATE
  WIN32 _LIB _SECURE_SCL=0 _CRT_SECURE_NO_WARNINGS
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)

# minizip (static) — vcxproj excludes miniunz.c/minizip.c, no PCH.
file(GLOB MINIZIP_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/minizip/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/minizip/*.h")
list(FILTER MINIZIP_SRCS EXCLUDE REGEX "mini(unz|zip)\\.c$")
add_library(minizip STATIC ${MINIZIP_SRCS})
target_compile_definitions(minizip PRIVATE
  WIN32 _LIB _CRT_SECURE_NO_WARNINGS
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)

# BugTrap (shared) — PCH stdafx.h; Debug binary is BugTrapD.dll per vcxproj.
file(GLOB BUGTRAP_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/bugtrap/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/bugtrap/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/bugtrap/*.rc"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/bugtrap/*.def")
add_library(BugTrap SHARED ${BUGTRAP_SRCS})
# Matches BugTrap.vcxproj: Debug DLL is BugTrapD.dll, but the import library
# stays BugTrap.lib (xrCore links it via `#pragma comment(lib, ...)`).
set_target_properties(BugTrap PROPERTIES
  OUTPUT_NAME "BugTrap" RUNTIME_OUTPUT_NAME_DEBUG "BugTrapD")
target_precompile_headers(BugTrap PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/bugtrap/bugtrap/stdafx.h")
target_compile_definitions(BugTrap PRIVATE
  WIN32 _WINDOWS _USRDLL BUGTRAP_EXPORTS _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
target_link_libraries(BugTrap PRIVATE minizip zlib ws2_32 comctl32 shlwapi version wininet)
target_compile_options(BugTrap PRIVATE /W4)

# ODE physics (shared, dSINGLE) — no PCH; ships with msvcdefs.def.
file(GLOB ODE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/ode/ode/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/ode/ode/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/ode/ode/src/*.h")
# de_padf_integration.cpp lives next to the vcxproj, not in ode/src.
list(APPEND ODE_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/ode/contrib/msvc7/ode_default/de_padf_integration.cpp")
add_library(ode SHARED ${ODE_SRCS}
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/ode/config/msvcdefs.def")
target_include_directories(ode PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/ode/include")
target_compile_definitions(ode PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS _USRDLL MSVC dSINGLE _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
  $<$<NOT:$<CONFIG:Debug>>:dNODEBUG>)

# OpenAL router (shared, output OpenAL32.dll) — no PCH.
file(GLOB OPENAL_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/openal/OpenAL-Windows/Router/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/openal/OpenAL-Windows/Router/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/openal/OpenAL-Windows/Router/*.rc")
add_library(openal32 SHARED ${OPENAL_SRCS})
set_target_properties(openal32 PROPERTIES OUTPUT_NAME "OpenAL32")
target_compile_definitions(openal32 PRIVATE
  WIN32 _WINDOWS _MBCS _USRDLL ROUTER_EXPORTS _SECURE_SCL=0
  $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
# System libs per Router.vcxproj (odbc pair is Debug-only there).
target_link_libraries(openal32 PRIVATE
  version winmm $<$<CONFIG:Debug>:odbc32;odbccp32>)

# jpeg (static, vendored CxImage copy) — no PCH.
file(GLOB JPEG_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/cximage/jpeg/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/cximage/jpeg/*.h")
add_library(jpeg STATIC ${JPEG_SRCS})
target_compile_definitions(jpeg PRIVATE
  WIN32 _WINDOWS _CRT_SECURE_NO_DEPRECATE
  $<$<CONFIG:Debug>:_DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)

# CxImage (shared) — no PCH. NOTE: needs the VS "MFC/ATL" component (_AFXDLL).
file(GLOB CXIMAGE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/cximage/CxImage/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/cximage/CxImage/*.h")
add_library(CxImage SHARED ${CXIMAGE_SRCS})
set_target_properties(CxImage PROPERTIES OUTPUT_NAME "CxImage")
target_include_directories(CxImage PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/cximage/jpeg")
# NOTE: the .sln maps this project's Mixed|Win32 to its Debug|Win32 config,
# so Mixed gets the Debug defines (no MIXED macro), like MSBuild.
target_compile_definitions(CxImage PRIVATE
  WIN32 CXIMAGE_BUILD CXIMAGE_AS_SHARED_LIBRARY
  JAS_WIN_MSVC_BUILD _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE _AFXDLL _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:_ALLOW_RTCc_IN_STL;_DEBUG;_ITERATOR_DEBUG_LEVEL=0>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(CxImage PRIVATE jpeg)

# crypto (static) — OpenSSL-derived sources, no PCH.
file(GLOB CRYPTO_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/crypto/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/crypto/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/crypto/openssl/src/*.c")
add_library(crypto STATIC ${CRYPTO_SRCS})
target_include_directories(crypto PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/crypto/openssl"
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_compile_definitions(crypto PRIVATE
  CRYPTO_BUILD _CRT_SECURE_NO_WARNINGS _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG;CRYPTO_AS_SHARED_LIBRARY>)

# LuaJIT 1.1.4 (shared, output lua.JIT.1.1.4.dll) — no PCH.
file(GLOB LUAJIT_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luajit/src/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luajit/src/*.h")
list(FILTER LUAJIT_SRCS EXCLUDE REGEX "/(lua|luac|print)\\.c$")
add_library(luajit SHARED ${LUAJIT_SRCS})
set_target_properties(luajit PROPERTIES OUTPUT_NAME "lua.JIT.1.1.4")
target_include_directories(luajit PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luajit/dynasm"
  "${XRAY_SDK_INCLUDE_DIR}" "${XRAY_ENGINE_DIR}/Include")
target_compile_definitions(luajit PRIVATE
  WIN32 _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG> $<$<CONFIG:Release>:NDEBUG>)

# luabind (shared, output luabind.beta7-devel.rc4.dll) — PCH pch.h.
file(GLOB LUABIND_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luabind/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luabind/luabind/src/*.cpp")
add_library(luabind SHARED ${LUABIND_SRCS})
set_target_properties(luabind PROPERTIES OUTPUT_NAME "luabind.beta7-devel.rc4")
target_include_directories(luabind PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luabind" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(luabind PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/3rd party/luabind/pch.h")
target_compile_definitions(luabind PRIVATE
  WIN32 LUABIND_BUILDING _SECURE_SCL=0 _HAS_ITERATOR_DEBUGGING=0 _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG> $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(luabind PRIVATE luajit)

# ------------------------------------------------------------- SDK from src ---
# libogg (static). NOTE: vcxproj uses the static CRT (MT); preserved here.
file(GLOB OGG_SRCS CONFIGURE_DEPENDS
  "${XRAY_SDK_DIR}/sources/libogg-1.1.4/src/*.c")
add_library(ogg STATIC ${OGG_SRCS})
set_target_properties(ogg PROPERTIES OUTPUT_NAME "libogg_static")
set_property(TARGET ogg PROPERTY MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:Debug>")
target_include_directories(ogg PUBLIC
  "${XRAY_SDK_DIR}/sources/libogg-1.1.4/include")
target_compile_definitions(ogg PRIVATE
  WIN32 _WINDOWS _USRDLL LIBOGG_EXPORTS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)

# libvorbis + libvorbisfile (static, MT like libogg).
file(GLOB VORBIS_SRCS CONFIGURE_DEPENDS
  "${XRAY_SDK_DIR}/sources/libvorbis-1.2.3/lib/*.c")
list(FILTER VORBIS_SRCS EXCLUDE REGEX "vorbisfile\\.c$")
# psytune.c / tone.c are tuning tools, not part of libvorbis per the vcxproj.
list(FILTER VORBIS_SRCS EXCLUDE REGEX "/(psytune|tone)\\.c$")
add_library(vorbis STATIC ${VORBIS_SRCS})
set_target_properties(vorbis PROPERTIES OUTPUT_NAME "libvorbis_static")
set_property(TARGET vorbis PROPERTY MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:Debug>")
target_include_directories(vorbis PUBLIC
  "${XRAY_SDK_DIR}/sources/libvorbis-1.2.3/include")
target_include_directories(vorbis PRIVATE
  "${XRAY_SDK_DIR}/sources/libogg-1.1.4/include" "${XRAY_SDK_INCLUDE_DIR}")
target_compile_definitions(vorbis PRIVATE
  WIN32 _WINDOWS _USRDLL LIBVORBIS_EXPORTS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(vorbis PRIVATE ogg)

add_library(vorbisfile STATIC
  "${XRAY_SDK_DIR}/sources/libvorbis-1.2.3/lib/vorbisfile.c")
set_target_properties(vorbisfile PROPERTIES OUTPUT_NAME "libvorbisfile_static")
set_property(TARGET vorbisfile PROPERTY MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:Debug>")
target_include_directories(vorbisfile PRIVATE
  "${XRAY_SDK_DIR}/sources/libvorbis-1.2.3/include"
  "${XRAY_SDK_DIR}/sources/libogg-1.1.4/include" "${XRAY_SDK_INCLUDE_DIR}")
target_compile_definitions(vorbisfile PRIVATE
  WIN32 _WINDOWS _USRDLL LIBVORBISFILE_EXPORTS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(vorbisfile PRIVATE vorbis ogg)

# libtheora (static, Win32 + OC_X86_ASM like the vcxproj Win32 configs).
file(GLOB THEORA_SRCS CONFIGURE_DEPENDS
  "${XRAY_SDK_DIR}/sources/libtheora-1.1.1/lib/*.c"
  "${XRAY_SDK_DIR}/sources/libtheora-1.1.1/lib/x86_vc/*.c")
list(FILTER THEORA_SRCS EXCLUDE REGEX "encoder_disabled\\.c$")
add_library(theora STATIC ${THEORA_SRCS})
set_target_properties(theora PROPERTIES OUTPUT_NAME "libtheora_static")
# The vcxproj uses MDd in Debug (unlike the engine-wide MD-everywhere); the
# .sln maps Mixed to Debug, so Mixed gets MDd too.
set_property(TARGET theora PROPERTY MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:Debug>DLL")
target_include_directories(theora PRIVATE
  "${XRAY_SDK_DIR}/sources/libtheora-1.1.1/include"
  "${XRAY_SDK_DIR}/sources/libtheora-1.1.1/lib"
  "${XRAY_SDK_DIR}/sources/libogg-1.1.4/include" "${XRAY_SDK_INCLUDE_DIR}")
target_compile_definitions(theora PRIVATE
  _CRT_SECURE_NO_DEPRECATE _CRT_NONSTDC_NO_DEPRECATE
  _BIND_TO_CURRENT_CRT_VERSION
  WIN32 _WINDOWS _USRDLL LIBTHEORA_EXPORTS OC_X86_ASM
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:_DEBUG;DEBUG> $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(theora PRIVATE ogg)

# OpenAutomate oalib (static).
add_library(oalib STATIC
  "${XRAY_SDK_DIR}/sources/OpenAutomate/src/OpenAutomate.c")
set_target_properties(oalib PROPERTIES OUTPUT_NAME "oalib")
# The vcxproj uses MDd in Debug (unlike the engine-wide MD-everywhere); the
# .sln maps Mixed to Debug, so Mixed gets MDd too.
set_property(TARGET oalib PROPERTY MSVC_RUNTIME_LIBRARY
  "MultiThreaded$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:Debug>DLL")
target_compile_definitions(oalib PRIVATE
  WIN32 _LIB
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:_DEBUG> $<$<CONFIG:Release>:NDEBUG>)

# 3rd-party/SDK libs keep the vcxproj default (exceptions on); the global
# /EHsc was stripped in cmake/xray.cmake, so re-apply it explicitly.
# (CxImage and crypto include xrCore.h, so they follow the engine
# exception policy instead — see xray_engine_eh below.)
foreach(_t IN ITEMS
    zlib minizip BugTrap ode openal32 jpeg luajit luabind
    ogg vorbis vorbisfile theora oalib)
  target_compile_options(${_t} PRIVATE /EHsc)
endforeach()

# ------------------------------------------------------------------ engine ---
# xrCore.h enforces: exceptions ON in Debug/Mixed (_CPPUNWIND required),
# exceptions OFF in Release (else #error). The vcxproj files encode this via
# per-config <ExceptionHandling>; CMake defaults to /EHsc everywhere, so the
# global default is stripped (cmake/xray.cmake) and every engine target links
# this helper. NOTE: Release must pass *no* /EH flag at all — even /EHsc-
# leaves _CPPUNWIND defined on modern MSVC, which still trips the #error.
add_library(xray_engine_eh INTERFACE)
target_compile_options(xray_engine_eh INTERFACE
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:/EHsc>
  # Every engine vcxproj sets ForceConformanceInForLoopScope=false; the code
  # relies on VC6-era for-loop scoping.
  /Zc:forScope-)

# Explicit Release LTCG for xrEngine, whose vcxproj sets
# WholeProgramOptimization=true. See cmake/xray.cmake for why CMake's
# built-in IPO is not used. (xrCore also sets it in its vcxproj, but its
# FS_impl.h inline-template pattern loses its out-of-line body under modern
# MSVC /GL+LTCG — the stock .sln Release build fails the exact same way —
# so xrCore links without /GL here.)
add_library(xray_release_ltcg INTERFACE)
target_compile_options(xray_release_ltcg INTERFACE $<$<CONFIG:Release>:/GL>)
target_link_options(xray_release_ltcg INTERFACE $<$<CONFIG:Release>:/LTCG>)

# xrCore (shared) — PCH stdafx.h. blackbox/* + Model.cpp build without PCH;
# DiagAssert.cpp only in Debug; xrDebug.cpp + LocatorAPI_Notifications are
# excluded in every vcxproj config.
file(GLOB_RECURSE XRCORE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/*.rc")
list(FILTER XRCORE_SRCS EXCLUDE REGEX "LocatorAPI_Notifications|/xrDebug\\.cpp$")
# Files present on disk but not part of the vcxproj build.
list(FILTER XRCORE_SRCS EXCLUDE REGEX "/(ELocatorAPI|lzo1x_d3|rt_lzo)\\.cpp$")
# DiagAssert is Debug-only in the vcxproj: keep it Debug-only here too.
list(FILTER XRCORE_SRCS EXCLUDE REGEX "blackbox/DiagAssert\\.cpp$")
list(APPEND XRCORE_SRCS
  "$<$<CONFIG:Debug>:${CMAKE_CURRENT_SOURCE_DIR}/xrCore/blackbox/DiagAssert.cpp>")
add_library(xrCore SHARED ${XRCORE_SRCS})
target_include_directories(xrCore PRIVATE "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrCore PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/stdafx.h")
file(GLOB XRCORE_NOPCH CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/blackbox/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/Model.cpp"
  # FS_impl.h includers: they implicitly instantiate IReaderBase<T>::find_chunk.
  # With /Yu, MSVC misattributes the instantiation to the PCH (which never
  # emits it) instead of the current TU -> LNK2019. Compiling these three TUs
  # without PCH (same as their plain #include "stdafx.h" compile) emits the
  # instantiation normally. The stock .sln build hits the same LNK2019.
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/FS.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/xrstring.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCore/stream_reader.cpp")
set_source_files_properties(${XRCORE_NOPCH} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrCore PRIVATE
  WIN32 _WINDOWS _USRDLL XRCORE_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrCore PRIVATE BugTrap)
target_compile_options(xrCore PRIVATE /W4)

# xrCDB (shared) — PCH stdafx.h. cl_raypick.cpp is ExcludedFromBuild in every
# vcxproj config (it needs the RAPID headers, absent from the repo).
file(GLOB XRCDB_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCDB/*")
list(FILTER XRCDB_SRCS EXCLUDE REGEX "/cl_raypick\\.cpp$")
add_library(xrCDB SHARED ${XRCDB_SRCS})
target_include_directories(xrCDB PRIVATE "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrCDB PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCDB/stdafx.h")
target_compile_definitions(xrCDB PRIVATE
  WIN32 _WINDOWS _USRDLL XRCDB_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrCDB PRIVATE xrCore)
target_compile_options(xrCDB PRIVATE /W4)

# xrCPU_Pipe (shared) — PCH StdAfx.h. The vcxproj excludes only the six
# BoneLerp/Skin*_3DNow!/SSE variants below (the remaining 3DNow! files build).
# NOTE: the vcxproj Debug/Release configs also define a bare "x2" macro; it is
# unused anywhere in the code and collides with WinSDK parameter names (e.g.
# GDI's x2 in Arc()), breaking windows.h, so it is deliberately dropped here.
file(GLOB XRCPUPIPE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCPU_Pipe/*")
list(FILTER XRCPUPIPE_SRCS EXCLUDE REGEX "/(xrBoneLerp(_3DNow!)?|xrSkin[12]W_(3DNow!|SSE))\\.cpp$")
add_library(xrCPU_Pipe SHARED ${XRCPUPIPE_SRCS})
target_include_directories(xrCPU_Pipe PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrCPU_Pipe PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrCPU_Pipe/stdafx.h")
target_compile_definitions(xrCPU_Pipe PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS _USRDLL XRCPU_PIPE_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrCPU_Pipe PRIVATE xrCore xrEngine)
target_compile_options(xrCPU_Pipe PRIVATE /W4)

# xrNetServer (shared) — PCH stdafx.h.
file(GLOB XRNETSERVER_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrNetServer/*")
add_library(xrNetServer SHARED ${XRNETSERVER_SRCS})
target_include_directories(xrNetServer PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrNetServer PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrNetServer/stdafx.h")
target_compile_definitions(xrNetServer PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS XR_NETSERVER_EXPORTS _MBCS
  $<$<CONFIG:Debug>:DEBUG>
  $<$<CONFIG:Mixed>:MIXED;DEBUG;_SECURE_SCL=0>
  $<$<CONFIG:Release>:NDEBUG;_SECURE_SCL=0>)
target_link_libraries(xrNetServer PRIVATE xrCore Ws2_32)
target_compile_options(xrNetServer PRIVATE /W4)

# xrParticles (shared) — PCH stdafx.h.
file(GLOB XRPARTICLES_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrParticles/*")
add_library(xrParticles SHARED ${XRPARTICLES_SRCS})
target_include_directories(xrParticles PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrParticles PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrParticles/stdafx.h")
target_compile_definitions(xrParticles PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS XR_PARTICLES_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrParticles PRIVATE xrCore)
target_compile_options(xrParticles PRIVATE /W4)

# xrXMLParser (shared) — PCH StdAfx.h; tiny* builds without PCH (matches
# Mixed/Release; Debug used PCH there, which is immaterial for correctness).
file(GLOB XRXMLPARSER_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrXMLParser/*")
add_library(xrXMLParser SHARED ${XRXMLPARSER_SRCS})
target_include_directories(xrXMLParser PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrXMLParser PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrXMLParser/StdAfx.h")
file(GLOB XRXMLPARSER_NOPCH CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrXMLParser/tiny*.cpp")
set_source_files_properties(${XRXMLPARSER_NOPCH} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrXMLParser PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS XRXMLPARSER_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrXMLParser PRIVATE xrCore)
target_compile_options(xrXMLParser PRIVATE /W4)

# xrGameSpy (shared) — PCH stdafx.h; GameSpy C sources build without PCH.
# The vcxproj has no Mixed config; the .sln maps Mixed to Debug (no MIXED).
file(GLOB_RECURSE XRGAMESPY_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/*.h")
# GameSpy_Patching.cpp is a stale copy (the vcxproj builds xrGameSpy_Patching
# instead; the real file lives in xrGame/gamespy). The gs*.c files below are
# ExcludedFromBuild in every vcxproj config.
list(FILTER XRGAMESPY_SRCS EXCLUDE REGEX "/(GameSpy_Patching\\.cpp|gsAssert\\.c|gsDebug\\.c|gsMemory\\.c|gsPlatform\\.c|gsPlatformSocket\\.c|gsPlatformThread\\.c|gsPlatformUtil\\.c|gsThreadWin32\\.c|Win32Common\\.c)$")
add_library(xrGameSpy SHARED ${XRGAMESPY_SRCS})
target_include_directories(xrGameSpy PRIVATE
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrGameSpy PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/stdafx.h")
file(GLOB_RECURSE XRGAMESPY_NOPCH CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/GameSpy/*.c")
set_source_files_properties(${XRGAMESPY_NOPCH} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrGameSpy PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS _USRDLL XRGAMESPY_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_compile_options(xrGameSpy PRIVATE /W4)

# xrSound (shared) — PCH stdafx.h; guids.cpp builds without PCH. The five
# files below exist on disk but are in no vcxproj config (dead CD-audio /
# dedicated variants), so they are excluded.
file(GLOB_RECURSE XRSOUND_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSound/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSound/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSound/*.h")
list(FILTER XRSOUND_SRCS EXCLUDE REGEX "/(MusicStream|SoundRender_CoreD|SoundRender_TargetD|xr_cda|xr_streamsnd)\\.(cpp|h)$")
add_library(xrSound SHARED ${XRSOUND_SRCS})
target_include_directories(xrSound PRIVATE "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrSound PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSound/stdafx.h")
set_source_files_properties(
  "${CMAKE_CURRENT_SOURCE_DIR}/xrSound/guids.cpp" PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrSound PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS _USRDLL XRSOUND_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrSound PRIVATE xrCore xrCDB openal32 vorbisfile vorbis ogg)
# stdafx.cpp does `#pragma comment(lib, "eax.lib")` (EAX SDK, prebuilt).
target_link_libraries(xrSound PRIVATE "${XRAY_SDK_LIB_DIR}/eax.lib")
target_compile_options(xrSound PRIVATE /W4)

# xrAPI (shared) — PCH stdafx.h; vcxproj is Debug/Release only. The .sln maps
# Mixed to Debug (no MIXED macro).
file(GLOB XRAPI_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrAPI/*")
add_library(xrAPI SHARED ${XRAPI_SRCS})
target_precompile_headers(xrAPI PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrAPI/stdafx.h")
target_compile_definitions(xrAPI PRIVATE
  WIN32 _WINDOWS _USRDLL XRAPI_EXPORTS _MBCS
  $<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:DEBUG>
  $<$<CONFIG:Release>:NDEBUG>)
target_compile_options(xrAPI PRIVATE /W4)

# xrD3D9-Null (shared) — stub D3D9 for dedicated servers; ships a .def.
# The vcxproj is Debug/Release only; the .sln maps Mixed to Release.
file(GLOB XRD3D9NULL_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrD3D9-Null/*")
add_library(xrD3D9-Null SHARED ${XRD3D9NULL_SRCS})
target_precompile_headers(xrD3D9-Null PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrD3D9-Null/stdafx.h")
target_compile_definitions(xrD3D9-Null PRIVATE
  WIN32 _WINDOWS _USRDLL XRD3D9NULL_EXPORTS _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)
target_compile_options(xrD3D9-Null PRIVATE /W4)

# xrRender_R1/R2/R3 (shared) — PCH stdafx.h (own dir). Each renderer compiles
# its own directory plus the shared Layers/xrRender (+blenders) and
# Layers/xrRenderDX9 (R1/R2) or Layers/xrRenderDX10 (R3) trees, plus two
# scripting helpers from xrEngine — mirroring the vcxproj ClCompile lists.
foreach(RENDER R1 R2 R3)
  if(RENDER STREQUAL "R1" AND NOT XRAY_BUILD_RENDER_R1)
    continue()
  elseif(RENDER STREQUAL "R2" AND NOT XRAY_BUILD_RENDER_R2)
    continue()
  elseif(RENDER STREQUAL "R3" AND NOT XRAY_BUILD_RENDER_R3)
    continue()
  endif()
  file(GLOB XRRENDER_SRCS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderPC_${RENDER}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderPC_${RENDER}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderPC_${RENDER}/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/blenders/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/blenders/*.h")
  if(RENDER STREQUAL "R3")
    file(GLOB_RECURSE XRRENDER_DX_SRCS CONFIGURE_DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderDX10/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderDX10/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderDX10/*.h")
    # R3 replaces these shared units with DX10-specific ones.
    list(FILTER XRRENDER_SRCS EXCLUDE REGEX "/(HW|ResourceManager_Resources|ResourceManager_Scripting|SH_RT|SH_Texture|Texture)\\.(cpp|h)$")
    # Two more R3 units live in xrRender subdirectories.
    list(APPEND XRRENDER_SRCS
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/Utils/dxHashHelper.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/Utils/dxHashHelper.h"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/Debug/dxPixEventWrapper.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/Debug/dxPixEventWrapper.h")
    # The own-dir du_sphere_part.cpp is stale (the vcxproj builds the shared
    # ../xrRender copy); compiling both duplicates the globals (LNK2005).
    list(FILTER XRRENDER_SRCS EXCLUDE REGEX "xrRenderPC_R3/du_sphere_part\\.cpp$")
  else()
    file(GLOB XRRENDER_DX_SRCS CONFIGURE_DEPENDS
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderDX9/*.cpp"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderDX9/*.c"
      "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderDX9/*.h")
    if(RENDER STREQUAL "R1")
      # Not in the R1 vcxproj.
      list(FILTER XRRENDER_SRCS EXCLUDE REGEX "/(r__occlusion|r__pixel_calculator|uber_deffer)\\.(cpp|h)$")
    endif()
    if(RENDER STREQUAL "R2")
      # The own-dir du_sphere_part.cpp is stale (the vcxproj builds the shared
      # ../xrRender copy); compiling both duplicates the globals (LNK2005).
      list(FILTER XRRENDER_SRCS EXCLUDE REGEX "xrRenderPC_R2/du_sphere_part\\.cpp$")
    endif()
  endif()
  list(APPEND XRRENDER_SRCS ${XRRENDER_DX_SRCS}
    "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/ai_script_lua_debug.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/ai_script_lua_extension.cpp")
  add_library("xrRender_${RENDER}" SHARED ${XRRENDER_SRCS})
  target_include_directories("xrRender_${RENDER}" PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderPC_${RENDER}"
    "${XRAY_SDK_INCLUDE_DIR}/dx"
    "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
  target_precompile_headers("xrRender_${RENDER}" PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRenderPC_${RENDER}/stdafx.h")
  # Doug Lea allocator is C and PCH-less (same as xrEngine/xrGame).
  set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/Layers/xrRender/doug_lea_memory_allocator.c"
    PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
  target_compile_definitions("xrRender_${RENDER}" PRIVATE
    _ALLOW_RTCc_IN_STL WIN32 _WINDOWS _USRDLL "XRRENDER_${RENDER}_EXPORTS"
    _SECURE_SCL=0 _MBCS
    $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:MIXED;DEBUG>
    $<$<CONFIG:Release>:NDEBUG>)
  target_link_libraries("xrRender_${RENDER}" PRIVATE xrEngine xrParticles xrAPI)
  target_compile_options("xrRender_${RENDER}" PRIVATE /W4)
  # The prebuilt nvapi.lib (2007-era) has no SAFESEH handlers; the vcxproj
  # links renders without SAFESEH as well.
  target_link_options("xrRender_${RENDER}" PRIVATE /SAFESEH:NO)
endforeach()

# Per-renderer extras (defines + prebuilt/SDK libs), matching the vcxproj files.
if(TARGET xrRender_R1)
  target_compile_definitions(xrRender_R1 PRIVATE
    $<$<CONFIG:Debug>:_ALLOW_RTCc_IN_STL>)
  target_link_libraries(xrRender_R1 PRIVATE
    "${XRAY_SDK_LIB_DIR}/nvapi.lib"
    $<$<NOT:$<CONFIG:Debug>>:${XRAY_SDK_LIB_DIR}/atimgpud_mtdll_x86.lib>)
endif()
if(TARGET xrRender_R2)
  target_compile_definitions(xrRender_R2 PRIVATE
    $<$<CONFIG:Debug>:USE_RESOURCE_DEBUGGER>)
  target_link_libraries(xrRender_R2 PRIVATE "${XRAY_SDK_LIB_DIR}/nvapi.lib")
endif()
if(TARGET xrRender_R3)
  target_compile_definitions(xrRender_R3 PRIVATE
    USE_DX10
    $<$<CONFIG:Debug>:MSAA_DX10_1;MSAA_SAMPLES=4;USE_MSAA;GBUFFER_OPTIMIZATION>)
  target_link_libraries(xrRender_R3 PRIVATE
    d3d10 "${XRAY_SDK_LIB_DIR}/d3dx10.lib" dxgi "${XRAY_SDK_LIB_DIR}/nvapi.lib")
endif()

# xrGame (shared) — PCH stdafx.h for gameplay code; xrServerEntities objects
# use pch_script.h in MSBuild, but CMake allows one PCH per target, so those
# compile without PCH (correctness-neutral). Warning level 2 per vcxproj.
file(GLOB_RECURSE XRGAME_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/*.cxx" "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/*.h")
file(GLOB XRSCRIPT_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrServerEntities/*.cpp")
set_source_files_properties(${XRSCRIPT_SRCS} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
# Compiled into xrGame.dll per the vcxproj but living elsewhere: GameSpy MD5
# and the Doug Lea allocator (the latter provides dlmalloc/dlrealloc used by
# script_storage.cpp; both are C && PCH-less, like the CdkeyDecode sources).
list(APPEND XRGAME_SRCS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/gamespy/md5c.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrServerEntities/doug_lea_memory_allocator.c")
# Files excluded in every vcxproj config, plus on-disk strays that no config
# lists (stale CS/CoP-era copies and variants).
list(FILTER XRGAME_SRCS EXCLUDE REGEX "/(ai_rat_fsm|debug_make_final_test|DynamicHeightMap|static_cast_checked_test|dcTriListCollider|UIEventsWnd|UIInventoryWnd|uiinventorywnd2|UIInventoryWnd3|UIOutfitInfo2|UIOutfitSlot|UITaskDescrWnd|UITaskItem|Artifact|ContextMenu|FractionState|GameMtlLib|GameMtlLib_Engine|IUWpnParams|LevelFogOfWar|SleepEffector|smart_cover_manager|stalker_animation_offsets|UIRankFraction|UIVideoPlayerWnd|WeaponHUD|WeaponMounted)\\.(cpp|h)$")
# Per-config sources (matching the vcxproj ExcludedFromBuild conditions).
list(FILTER XRGAME_SRCS EXCLUDE REGEX "/(level_map_locations|profiler|UIDebugFonts)\\.cpp$")
list(FILTER XRSCRIPT_SRCS EXCLUDE REGEX "/script_stack_tracker\\.cpp$")
list(APPEND XRGAME_SRCS
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:${CMAKE_CURRENT_SOURCE_DIR}/xrGame/profiler.cpp>"
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:${CMAKE_CURRENT_SOURCE_DIR}/xrGame/ui/UIDebugFonts.cpp>"
  "$<$<NOT:$<CONFIG:Debug>>:${CMAKE_CURRENT_SOURCE_DIR}/xrGame/level_map_locations.cpp>")
list(APPEND XRSCRIPT_SRCS
  "$<$<OR:$<CONFIG:Debug>,$<CONFIG:Mixed>>:${CMAKE_CURRENT_SOURCE_DIR}/xrServerEntities/script_stack_tracker.cpp>")
add_library(xrGame SHARED ${XRGAME_SRCS} ${XRSCRIPT_SRCS})
target_include_directories(xrGame PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGame"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrServerEntities"
  "${XRAY_SDK_INCLUDE_DIR}/stlport" "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrGame PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/stdafx.h")
# Pure-C GameSpy sources: no PCH (a forced C++ PCH breaks C compilation).
file(GLOB XRGAME_C_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/gamespy/CdkeyDecode/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGameSpy/gamespy/md5c.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrServerEntities/doug_lea_memory_allocator.c")
set_source_files_properties(${XRGAME_C_SRCS} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
# PHObject.cpp instantiates the CPHItemList template; same /Yu emission issue
# as xrCore's FS_impl.h, same remedy (see XRCORE_NOPCH comment).
set_source_files_properties(
  "${CMAKE_CURRENT_SOURCE_DIR}/xrGame/PHObject.cpp"
  PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
target_compile_definitions(xrGame PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS _USRDLL XRGAME_EXPORTS
  dSINGLE MSVC _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:DEBUG;MIXED>
  $<$<CONFIG:Release>:NDEBUG>)
target_link_libraries(xrGame PRIVATE
  crypto CxImage luabind luajit ode xrAPI xrEngine xrGameSpy xrNetServer xrXMLParser)
# screenshot_manager.cpp does `#pragma comment(lib, "jpeg.lib")`.
target_link_libraries(xrGame PRIVATE jpeg)
target_compile_options(xrGame PRIVATE /W2 /MP)

# xrEngine (exe) — PCH stdafx.h. The six MSBuild configs
# (Debug/Mixed/Release x Dedicated) collapse into one target + the
# XRAY_DEDICATED_SERVER option; Mixed adds INGAME_EDITOR like the vcxproj.
file(GLOB_RECURSE XRENGINE_SRCS CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/*.c"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/*.rc")
list(FILTER XRENGINE_SRCS EXCLUDE REGEX "editor_environment_manager_properties\\.cpp$")
# xrLoadSurface.cpp exists on disk but is in no vcxproj config.
list(FILTER XRENGINE_SRCS EXCLUDE REGEX "/xrLoadSurface\\.cpp$")
add_executable(xrEngine WIN32 ${XRENGINE_SRCS})
# xrEngine.exe exports the engine API (ENGINE_API) and game/render DLLs link
# its import library. ENABLE_EXPORTS makes CMake pass xrEngine.lib by full
# path to dependents (otherwise they get a bare name that is not on LIBPATH).
set_target_properties(xrEngine PROPERTIES ENABLE_EXPORTS TRUE)
target_include_directories(xrEngine PRIVATE "${XRAY_SDK_INCLUDE_DIR}")
target_precompile_headers(xrEngine PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/stdafx.h")
set_source_files_properties(
  "${CMAKE_CURRENT_SOURCE_DIR}/xrEngine/doug_lea_memory_allocator.c"
  PROPERTIES SKIP_PRECOMPILE_HEADERS ON COMPILE_OPTIONS "/wd4127;/wd4267")
target_compile_definitions(xrEngine PRIVATE
  _ALLOW_RTCc_IN_STL WIN32 _WINDOWS ENGINE_BUILD _SECURE_SCL=0 _MBCS
  $<$<CONFIG:Debug>:DEBUG> $<$<CONFIG:Mixed>:INGAME_EDITOR;DEBUG;MIXED>
  $<$<CONFIG:Release>:NDEBUG>
  $<$<BOOL:${XRAY_DEDICATED_SERVER}>:DEDICATED_SERVER>)
target_link_libraries(xrEngine PRIVATE
  xrCore xrCDB xrSound xrAPI luabind luajit ogg theora oalib
  msacm32 vfw32 xray_release_ltcg)
target_compile_options(xrEngine PRIVATE /W4 /Zm200)

# Exception-handling policy (see xray_engine_eh above) for the engine targets
# listed below (xrD3D9-Null uses xray_engine_eh_release instead, see below).
add_library(xray_engine_eh_release INTERFACE)
target_compile_options(xray_engine_eh_release INTERFACE $<$<CONFIG:Debug>:/EHsc>)
foreach(_t IN ITEMS
    xrCore xrCDB xrCPU_Pipe xrNetServer xrParticles xrXMLParser xrGameSpy
    xrSound xrAPI xrRender_R1 xrRender_R2 xrRender_R3 xrGame xrEngine
    CxImage crypto)
  if(TARGET ${_t})
    target_link_libraries(${_t} PRIVATE xray_engine_eh)
    # Mirrors the vcxproj AdditionalLibraryDirectories (build libraries +
    # SDK libraries): resolves `#pragma comment(lib, ...)` deps by bare name
    # (xrCore.lib, xrCDB.lib, eax.lib, ...) just like MSBuild.
    target_link_directories(${_t} PRIVATE
      "${CMAKE_BINARY_DIR}/lib/$<CONFIG>" "${XRAY_SDK_LIB_DIR}")
  endif()
endforeach()
# NOTE: xrCore is deliberately excluded from LTCG. Its FS_impl.h
# inline-template pattern loses its out-of-line body under modern MSVC
# /GL+LTCG (LNK2001 for IReaderBase<IReader>::find_chunk); the stock .sln
# Release build fails the exact same way. Native codegen emits the symbol
# correctly, so xrCore links without /GL here.
foreach(_t IN ITEMS xrEngine)
  if(TARGET ${_t})
    target_link_libraries(${_t} PRIVATE xray_release_ltcg)
  endif()
endforeach()
target_link_libraries(xrD3D9-Null PRIVATE xray_engine_eh_release)
# Pragma-driven deps made explicit (the vcxproj files relied on
# `#pragma comment(lib, ...)` + AdditionalLibraryDirectories instead):
# ximacfg.h -> xrcore.lib, crypto.cpp -> xrCore.lib. Linking the targets also
# fixes build order, which the .sln only provided via solution build order.
target_link_libraries(CxImage PRIVATE xrCore)
target_link_libraries(crypto PRIVATE xrCore)

# `cmake --install` support: drop the built runtime straight into a game
# installation. Point CMAKE_INSTALL_PREFIX at the game root:
#   cmake -S . -B build -G "Visual Studio 17 2022" -A Win32 -DCMAKE_INSTALL_PREFIX="C:/Games/S.T.A.L.K.E.R. - Clear Sky"
#   cmake --build build --config Release
#   cmake --install build --config Release   (copies DLLs/EXE to <game>/bin)
set(_xray_runtime_targets
  BugTrap ode openal32 CxImage luajit luabind
  xrCore xrCDB xrCPU_Pipe xrNetServer xrParticles xrXMLParser xrGameSpy
  xrSound xrAPI xrD3D9-Null xrGame xrEngine)
foreach(_r R1 R2 R3)
  if(TARGET xrRender_${_r})
    list(APPEND _xray_runtime_targets xrRender_${_r})
  endif()
endforeach()
install(TARGETS ${_xray_runtime_targets} RUNTIME DESTINATION bin)
