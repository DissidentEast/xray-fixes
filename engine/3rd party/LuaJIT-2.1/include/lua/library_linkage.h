////////////////////////////////////////////////////////////////////////////
//	Module 		: library_linkage.h
//	Created 	: 14.04.2007
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : library linkage file
////////////////////////////////////////////////////////////////////////////

#ifndef LUA_LIBRARY_LINKAGE_H_INCLUDED
#define LUA_LIBRARY_LINKAGE_H_INCLUDED

// CMake (both Win32 and x64) and the legacy engine.sln all ship the scripting
// runtime under the historical LuaJIT 1.1.4 file name; since the 64-bit port
// the DLL content is LuaJIT 2.1 (GC64 on x64) — see "3rd party/LuaJIT-2.1".
#pragma comment(lib, "lua.JIT.1.1.4.lib")

#endif // #ifndef LUA_LIBRARY_LINKAGE_H_INCLUDED
