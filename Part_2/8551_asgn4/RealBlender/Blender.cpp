// Blender.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "Blender.h"
#include "emmintrin.h"

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
		movd		mm2, alpha				// 00 00 00 0A
		punpcklwd	mm2, mm2				// 00 00 0A 0A
		punpcklwd	mm2, mm2				// 0A 0A 0A 0A
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
		punpcklbw	mm1, mm6				// mm1 = (0R 0G 0B 0A)
		movq		mm3, mm1				// mm3 = mm1: dst (0R 0G 0B 0A)
		psubw		mm0, mm1				// mm0 = mm0 - mm1
		psllw		mm3, 8					// mm3 = mm1 * 256
		pmullw		mm0, mm2				// mm0 = (src - dst) * alpha
		paddw		mm0, mm3				// mm0 = (src - dst) * alpha + dst * 256
		psrlw		mm0, 8				    // mm0 = ((src - dst) * alpha + dst * 256) / 256
		// SECOND PIXEL
		punpckhbw	mm4, mm6				// mm4 = (0R 0G 0B 0A)
		punpckhbw	mm5, mm6				// mm5 = (0R 0G 0B 0A)
		movq		mm3, mm5				// mm3 = mm5: dst (0R 0G 0B 0A)
		psubw		mm4, mm5				// mm4 = mm4 - mm5
		psllw		mm3, 8				    // mm3 = mm5 * 256
		pmullw		mm4, mm2				// mm4 = (src - dst) * alpha
		paddw		mm4, mm3				// mm4 = (src - dst) * alpha + dst * 256
		psrlw		mm4, 8				    // mm4 = ((src - dst) * alpha + dst * 256) / 256
		packuswb	mm0, mm4				// mm0 = RG BA RG BA
		movq[edi + ebx * 8], mm0		    // dst = mm0
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

	unsigned short alpha_byte = (unsigned short)(blend_factor * 255);
	__m128i alpha;
	alpha.m128i_u16[0] = alpha_byte;
	alpha.m128i_u16[1] = alpha_byte;
	alpha.m128i_u16[2] = alpha_byte;
	alpha.m128i_u16[3] = alpha_byte;
	alpha.m128i_u16[4] = alpha_byte;
	alpha.m128i_u16[5] = alpha_byte;
	alpha.m128i_u16[6] = alpha_byte;
	alpha.m128i_u16[7] = alpha_byte;
	//alpha = 0A 0A 0A 0A 0A 0A 0A 0A
	__m128i zero;
	zero.m128i_u64[0] = 0;
	zero.m128i_u64[1] = 0;
	//zero  = 00 00 00 00 00 00 00 00
	for (int y = 0; y < blendTo->height; ++y)
	{
		unsigned char* dst = (unsigned char*)finalImage->linearData + (y * finalImage->stride);
		unsigned char* src = (unsigned char*)original->linearData + (y * original->stride);
		unsigned char* blend = (unsigned char*)blendTo->linearData + (y * blendTo->stride);
		for (int x = 0; x < blendTo->width; x += 4)
		{
			//FIRST AND SECOND PIXELS!
			__m128i src_data   = _mm_load_si128(reinterpret_cast<__m128i*>(src)); //src_data = s(RGBA RGBA RGBA RGBA)
			__m128i blend_data = _mm_load_si128(reinterpret_cast<__m128i*>(blend)); //blend_data = b(RGBA RGBA RGBA RGBA)
			
			__m128i src_unpacked = _mm_unpacklo_epi8(src_data, zero); //src_unpacked = s(0R 0G 0B 0A 0R 0G 0B 0A)
			__m128i blend_unpacked = _mm_unpacklo_epi8(blend_data, zero); //blend_unpacked = b(0R 0G 0B 0A 0R 0G 0B 0A)
			src_unpacked = _mm_sub_epi16(src_unpacked, blend_unpacked); //src_unpacked = s-b(RR GG BB AA RR GG BB AA)
			blend_unpacked = _mm_slli_epi16(blend_unpacked, 8); //b(R0 G0 B0 A0 R0 G0 B0 A0)
			src_unpacked = _mm_mullo_epi16(src_unpacked, alpha);
			__m128i result_first = _mm_add_epi16(blend_unpacked, src_unpacked); //result = RR GG BB AA RR GG BB AA
			result_first = _mm_srli_epi16(result_first, 8);//result = 0R 0G 0B 0A 0R 0G 0B 0A
			//THIRD AND FOURTH PIXELS!
			src_unpacked = _mm_unpackhi_epi8(src_data, zero); //src_unpacked = s(0R 0G 0B 0A 0R 0G 0B 0A)
			blend_unpacked = _mm_unpackhi_epi8(blend_data, zero); //blend_unpacked = b(0R 0G 0B 0A 0R 0G 0B 0A)
			src_unpacked = _mm_sub_epi16(src_unpacked, blend_unpacked); //src_unpacked = s-b(RR GG BB AA RR GG BB AA)
			blend_unpacked = _mm_slli_epi16(blend_unpacked, 8); //b(R0 G0 B0 A0 R0 G0 B0 A0)
			src_unpacked = _mm_mullo_epi16(src_unpacked, alpha);
			__m128i result_second = _mm_add_epi16(blend_unpacked, src_unpacked); //result = RR GG BB AA RR GG BB AA
			result_second = _mm_srli_epi16(result_second, 8);//result = 0R 0G 0B 0A 0R 0G 0B 0A
			__m128i big_result = _mm_packus_epi16(result_first, result_second);
			_mm_store_si128(reinterpret_cast<__m128i*>(dst), big_result);
			dst += 16; //4 pixels at a time.
			src += 16; //4 pixels at a time.
			blend += 16; //4 pixels at a time.
		}
	}

	QueryPerformanceCounter(&after);
	elapsed_micros.QuadPart = after.QuadPart - before.QuadPart;
	return  (int)(elapsed_micros.QuadPart * 1000000 / frequency.QuadPart);
}
