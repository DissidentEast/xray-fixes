// xrCPU_Pipe.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#pragma hdrstop

#pragma comment(lib,"xrEngine.lib")

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

extern xrSkin1W			xrSkin1W_x86;
extern xrSkin1W			xrSkin1W_3DNow;
// extern xrSkin1W		xrSkin1W_SSE;
extern xrSkin2W			xrSkin2W_x86;
extern xrSkin2W			xrSkin2W_SSE;
extern xrSkin2W			xrSkin2W_3DNow;

extern xrSkin3W			xrSkin3W_x86;
extern xrSkin4W			xrSkin4W_x86;
//extern xrBoneLerp		xrBoneLerp_x86;
//extern xrBoneLerp		xrBoneLerp_3DNow;
extern xrM44_Mul		xrM44_Mul_x86;
extern xrM44_Mul		xrM44_Mul_3DNow;
extern xrM44_Mul		xrM44_Mul_SSE;
extern xrTransfer		xrTransfer_x86;
#ifndef _M_AMD64
// x64 drops the hand-written asm units; their dispatch entries stay NULL
// (nothing in the CS renderer reads them anyway).
extern xrMemCopy_8b		xrMemCopy_MMXSSE3DNow;
extern xrMemFill_32b	xrMemFill32_MMX;
#endif
extern xrMemCopy_8b		xrMemCopy_x86;
#ifdef _M_AMD64
// The plain CopyMemory wrapper lived in the excluded asm unit.
void	__stdcall	xrMemCopy_x86	(LPVOID dest, const void* src, u32 n)	{ CopyMemory(dest,src,n); }
#endif


extern "C" {
	__declspec(dllexport) void	__cdecl	xrBind_PSGP	(xrDispatchTable* T, DWORD dwFeatures)
	{
		// analyze features
		// DWORD dwFeatures = CPU::ID.feature & CPU::ID.os_support;

		if(strstr(strlwr(GetCommandLine()),"-x86"))	dwFeatures &= ~(_CPU_FEATURE_SSE+_CPU_FEATURE_3DNOW);

		// generic
		T->skin1W	= xrSkin1W_x86;
		T->skin2W	= xrSkin2W_x86;
		T->skin3W	= xrSkin3W_x86;
		T->skin4W	= xrSkin4W_x86;
		// T->blerp	= xrBoneLerp_x86;
		T->m44_mul	= xrM44_Mul_x86;
		T->transfer = xrTransfer_x86;
		T->memCopy	= xrMemCopy_x86;
		T->memFill	= NULL;
#ifdef _M_AMD64
		T->memFill32= NULL;
#else
		T->memFill32= xrMemFill32_MMX;
#endif
		
		// SSE
		if (dwFeatures & _CPU_FEATURE_SSE) {
#ifndef _M_AMD64
			T->memCopy	= xrMemCopy_MMXSSE3DNow;
#endif
			//T->skin2W	= xrSkin2W_SSE;
		}

		// 3dnow!
		if (dwFeatures & _CPU_FEATURE_3DNOW) {
 			//T->skin1W	= xrSkin1W_3DNow;
			// T->blerp	= xrBoneLerp_3DNow;
#ifndef _M_AMD64
			T->memCopy	= xrMemCopy_MMXSSE3DNow;
#endif
			//T->skin2W	= xrSkin2W_3DNow;
		}
	}
};
