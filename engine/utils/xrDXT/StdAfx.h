// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//
// Third generation by Oles.

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once

#include "../../xrCore/xrCore.h"
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#define ENGINE_API
#define XR_EPROPS_API
#define ECORE_API

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
	((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |   \
	((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif //defined(MAKEFOURCC)

#pragma comment(lib,"xrCore.lib")
// NOTE: the prebuilt NVIDIA nvDXTlibMTDLL.lib in this folder targets the
// VS2003 CRT and cannot link (35 unresolved std:: imports); the pragma
// that pulled it in is removed here. See the xrDXT block in
// engine/utils/CMakeLists.txt for the full story.

#pragma warning( disable : 4995 )

#endif