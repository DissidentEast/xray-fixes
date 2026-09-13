#include "stdafx.h"

#ifdef _M_AMD64
// MSVC x64 has no inline asm. The x87 original computes D = M1^T * M2 with
// every destination row reusing the M2 column held in the x87 stack; the
// double accumulator below mirrors its 80-bit intermediate precision.
void	__stdcall	xrM44_Mul_x86	(_matrix<float>* pfD, _matrix<float>* pfM1, _matrix<float>* pfM2)
{
	const _matrix<float>&	M1	= *pfM1;
	const _matrix<float>&	M2	= *pfM2;
	_matrix<float>&			D	= *pfD;

	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			double	acc	= 0.0;
			for (int r = 0; r < 4; ++r)
				acc	+= double(M1.m[r][i]) * double(M2.m[r][j]);
			D.m[i][j]	= float(acc);
		}
}
#else
void	__stdcall	xrM44_Mul_x86	(_matrix<float>* pfD, _matrix<float>* pfM1, _matrix<float>* pfM2)
{
    __asm
    {
        push        edx

        mov         eax, pfM2
        mov         ebx, pfM1
        mov         edx, pfD

        fld         dword ptr[ eax ]
        fld         dword ptr[ eax + 4 ]
        fld         dword ptr[ eax + 8 ]
        fld         dword ptr[ eax + 12 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 4 ]
        fstp        dword ptr[ edx ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 12 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 8 ]





        fld         dword ptr[ eax + 16 ]
        fld         dword ptr[ eax + 20 ]
        fld         dword ptr[ eax + 24 ]
        fld         dword ptr[ eax + 28 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 20 ]
        fstp        dword ptr[ edx + 16 ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 28 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 24 ]





        fld         dword ptr[ eax + 32 ]
        fld         dword ptr[ eax + 36 ]
        fld         dword ptr[ eax + 40 ]
        fld         dword ptr[ eax + 44 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 36 ]
        fstp        dword ptr[ edx + 32 ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 44 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 40 ]





        fld         dword ptr[ eax + 48 ]
        fld         dword ptr[ eax + 52 ]
        fld         dword ptr[ eax + 56 ]
        fld         dword ptr[ eax + 60 ]
        
        fld         dword ptr[ ebx ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 4 ]
        fmul        st(0), st(5)
        fld         dword ptr[ ebx + 16 ]         
        fld         dword ptr[ ebx + 20 ]
        fmul        st(0), st(6)
        faddp       st(2), st(0)
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 32 ]         
        fld         dword ptr[ ebx + 36 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fld         dword ptr[ ebx + 48 ]         
        fld         dword ptr[ ebx + 52 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fstp        dword ptr[ edx + 52 ]
        fstp        dword ptr[ edx + 48 ]


        fld         dword ptr[ ebx + 12 ]
        fmul        st(0), st(4)
        fld         dword ptr[ ebx + 8 ]
        fmulp       st(5), st(0)
        fld         dword ptr[ ebx + 24 ]
        fld         dword ptr[ ebx + 28 ]
        fmul        st(0), st(5)
        faddp       st(2), st(0)
        fmulp       st(4), st(0)
        fld         dword ptr[ ebx + 40 ]
        fld         dword ptr[ ebx + 44 ]
        fmul        st(0), st(4)
        faddp       st(2), st(0)
        fmulp       st(3), st(0)
        fld         dword ptr[ ebx + 56 ]
        fld         dword ptr[ ebx + 60 ]         
        fmul        st(0), st(3)
        faddp       st(2), st(0)
        fmulp       st(2), st(0)
        fstp        dword ptr[ edx + 60 ]
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        faddp       st(1), st(0)
        fstp        dword ptr[ edx + 56 ]


        pop         edx
    }
}
#endif // _M_AMD64
