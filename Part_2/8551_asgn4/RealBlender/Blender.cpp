// Blender.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "Blender.h"

#include <Windows.h>

EXTERN_C __declspec(dllexport) int __stdcall Blend(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor) {
	float inv_blend_factor = 1.0f - blend_factor;

	LARGE_INTEGER before, after, frequency, elapsed_micros;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&before);

	for (int i = 0; i < blendTo->height; i++) {
		unsigned char* original_scanline_start = (unsigned char*)original->linearData + (original->stride * i);
		unsigned char* blendTo_scanline_start = (unsigned char*)blendTo->linearData + (blendTo->stride * i);
		unsigned char* finalImage_scanline_start = (unsigned char*)finalImage->linearData + (finalImage->stride * i);
		for (int j = 0; j < blendTo->width * 4; j += 4) {
			unsigned char* original_pixel = original_scanline_start + j;
			unsigned char* blend_pixel = blendTo_scanline_start + j;
			unsigned char* final_pixel = finalImage_scanline_start + j;
			for (int ch = 0; ch < 3; ch++) {
				final_pixel[ch] = (unsigned char)(original_pixel[ch] * blend_factor + blend_pixel[ch] * inv_blend_factor);
			}
		}
	}
	QueryPerformanceCounter(&after);
	elapsed_micros.QuadPart = after.QuadPart - before.QuadPart;
	return (int)(elapsed_micros.QuadPart * 1000000 / frequency.QuadPart);
}

EXTERN_C __declspec(dllexport) int __stdcall Blend_MMX(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor) {
	float inv_blend_factor = 1.0f - blend_factor;

	LARGE_INTEGER before, after, frequency, elapsed_micros;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&before);

	int	wmul4 = blendTo->stride;
	int w = blendTo->width / 2;

	unsigned char* dst = (unsigned char*)finalImage->linearData;
	unsigned char* src = (unsigned char*)original->linearData;
	unsigned char* blend = (unsigned char*)blendTo->linearData;
	unsigned int alpha = (unsigned int)(blend_factor * 255);
	int h = blendTo->height;



	_asm {
		// For each pixel: dst = (src_alpha * (src-dst)  + dst * 256) / 256
		mov			edi, dst
		mov			esi, src
		mov         eax, blend
		mov			edx, h
		pxor		mm6, mm6
		pxor		mm7, mm7
	scan_loop :
		mov			ecx, w
		xor			ebx, ebx
	pix_loop :
		movq		mm4, [esi + ebx * 8]		// mm4 = src (RG BA RG BA)
		movq		mm5, [eax + ebx * 8]		// mm5 = dst (RG BA RG BA)
		// FIRST PIXEL
		movq		mm0, mm4				// mm0 = src (-- -- RG BA)
		movq		mm1, mm5				// mm1 = dst (-- -- RG BA)
		punpcklbw	mm0, mm6				// mm0 = (0R 0G 0B 0A)
		punpcklbw	mm1, mm7				// mm1 = (0R 0G 0B 0A)
		movd		mm2, alpha				// 00 00 00 0A
		punpcklwd	mm2, mm2				// 00 00 0A 0A
		punpcklwd	mm2, mm2				// 0A 0A 0A 0A
		movq		mm3, mm1				// mm3 = mm1: dst (0R 0G 0B 0A)
		psubw		mm0, mm1				// mm0 = mm0 - mm1
		psllw		mm3, 8					// mm3 = mm1 * 256
		pmullw		mm0, mm2				// mm0 = (src - dst) * alpha
		paddw		mm0, mm3				// mm0 = (src - dst) * alpha + dst * 256
		psrlw		mm0, 8				// mm0 = ((src - dst) * alpha + dst * 256) / 256
		packuswb	mm0, mm6				// mm0 = RGBA
		// SECOND PIXEL
		punpckhbw	mm4, mm6				// mm4 = (0R 0G 0B 0A)
		punpckhbw	mm5, mm7				// mm5 = (0R 0G 0B 0A)
		movq		mm3, mm5				// mm3 = mm5: dst (0R 0G 0B 0A)
		psubw		mm4, mm5				// mm4 = mm4 - mm5
		psllw		mm3, 8				// mm3 = mm5 * 256
		pmullw		mm4, mm2				// mm4 = (src - dst) * alpha
		paddw		mm4, mm3				// mm4 = (src - dst) * alpha + dst * 256
		psrlw		mm4, 8				// mm4 = ((src - dst) * alpha + dst * 256) / 256
		packuswb	mm4, mm6				// mm4 = RGBA
		punpckldq	mm0, mm4				// mm0 = RG BA RG BA
		movq[edi + ebx * 8], mm0		// dst = mm0
		inc			ebx
		// REPEAT
		loop		pix_loop
		mov			ebx, wmul4
		add         eax, ebx
		add			esi, ebx
		add			edi, ebx
		dec			edx
		jnz			scan_loop
		emms
	}
	//_mm_empty();
	QueryPerformanceCounter(&after);
	elapsed_micros.QuadPart = after.QuadPart - before.QuadPart;
	return  (int)(elapsed_micros.QuadPart * 1000000 / frequency.QuadPart);
}


EXTERN_C __declspec(dllexport) int __stdcall Blend_SSE(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor) {
	float inv_blend_factor = 1.0f - blend_factor;

	LARGE_INTEGER before, after, frequency, elapsed_micros;
	QueryPerformanceFrequency(&frequency);
	QueryPerformanceCounter(&before);


	int	wmul4 = blendTo->stride;
	int w = blendTo->width / 2;

	unsigned char* dst = (unsigned char*)finalImage->linearData;
	unsigned char* src = (unsigned char*)original->linearData;
	unsigned char* blend = (unsigned char*)blendTo->linearData;
	unsigned int alpha = (unsigned int)(blend_factor * 255);
	int h = blendTo->height;

	_asm {
		// For each pixel: dst = (src_alpha * (src-dst)  + dst * 256) / 256
			mov			edi, dst
			mov			esi, src
			mov         eax, blend
			mov			edx, h
			pxor		mm6, mm6
			pxor		mm7, mm7
		scan_loop :
			mov			ecx, w
			xor			ebx, ebx
		pix_loop :
			movq		mm4, [esi + ebx * 8]		// mm4 = src (RG BA RG BA)
			movq		mm5, [eax + ebx * 8]		// mm5 = blend (RG BA RG BA)
			// FIRST PIXEL
			movq		mm0, mm4				// mm0 = 00 00 RG BA
			movq		mm1, mm5				// mm1 = 00 00 RG BA
			punpcklbw	mm0, mm6				// mm0 = (0R 0G 0B 0A)
			punpcklbw	mm1, mm7				// mm0 = (0R 0G 0B 0A)
			movd		mm2, alpha				// 00 00 00 0A
			punpcklwd	mm2, mm2				// 00 00 0A 0A
			punpcklwd	mm2, mm2				// 0A 0A 0A 0A
			movq		mm3, mm1				// mm3 = mm1
			psubw		mm0, mm1				// mm0 = mm0 - mm1
			psllw		mm3, 8				// mm3 = mm1 * 256
			pmullw		mm0, mm2				// mm0 = (src-dst)*alpha
			paddw		mm0, mm3				// mm0 = (src-dst)*alpha+dst*256
			psrlw		mm0, 8				// mm0 = ((src - dst) * alpha + dst * 256) / 256
			// SECOND PIXEL
			punpckhbw	mm5, mm7				// mm5 = (0R 0G 0B 0A)
			punpckhbw	mm4, mm6				// mm4 = (0R 0G 0B 0A)
			movq		mm3, mm5				// mm3 = mm5
			psllw		mm3, 8				// mm3 = mm5 * 256
			psubw		mm4, mm5				// mm4 = mm4 - mm5
			pmullw		mm4, mm2				// mm4 = (src-dst)*alpha
			paddw		mm4, mm3				// mm4 = (src-dst)*alpha+dst*256
			psrlw		mm4, 8				// mm4 = ((src - dst) * alpha + dst * 256) / 256
			packuswb	mm0, mm4				// mm0 = RG BA RG BA
			movq[edi + ebx * 8], mm0		// dst = mm0
			inc			ebx
			loop		pix_loop
			//
			mov			ebx, wmul4
			add			esi, ebx
			add	        eax, ebx
			add			edi, ebx
			dec			edx
			jnz			scan_loop
			emms
	}

	QueryPerformanceCounter(&after);
	elapsed_micros.QuadPart = after.QuadPart - before.QuadPart;
	return  (int)(elapsed_micros.QuadPart * 1000000 / frequency.QuadPart);
}
