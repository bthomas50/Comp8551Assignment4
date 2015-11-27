// Blender.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "Blender.h"
#include "emmintrin.h"
#include "mmintrin.h"

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

	unsigned short alpha_byte = (unsigned short)(blend_factor * 255);
	__m64 alpha;
	alpha.m64_u16[0] = alpha_byte;
	alpha.m64_u16[1] = alpha_byte;
	alpha.m64_u16[2] = alpha_byte;
	alpha.m64_u16[3] = alpha_byte;
	//alpha = 0A 0A 0A 0A 
	__m64 zero;
	zero.m64_u64 = 0;
	//zero  = 00 00 00 00 

	for (int y = 0; y < blendTo->height; ++y)
	{
		unsigned char* dst = (unsigned char*)finalImage->linearData + (y * finalImage->stride);
		unsigned char* src = (unsigned char*)original->linearData + (y * original->stride);
		unsigned char* blend = (unsigned char*)blendTo->linearData + (y * blendTo->stride);
		for (int x = 0; x < blendTo->width; x += 2)
		{
			//FIRST AND SECOND PIXELS!
			__m64 src_data = *reinterpret_cast<__m64*>(src); //src_data = s(RGBA RGBA RGBA RGBA)
			__m64 blend_data = *reinterpret_cast<__m64*>(blend); //blend_data = b(RGBA RGBA RGBA RGBA)

			__m64 src_unpacked = _mm_unpacklo_pi8(src_data, zero); //src_unpacked = s(0R 0G 0B 0A 0R 0G 0B 0A)
			__m64 blend_unpacked = _mm_unpacklo_pi8(blend_data, zero); //blend_unpacked = b(0R 0G 0B 0A 0R 0G 0B 0A)
			src_unpacked = _mm_sub_pi16(src_unpacked, blend_unpacked); //src_unpacked = s-b(RR GG BB AA RR GG BB AA)
			blend_unpacked = _mm_slli_pi16(blend_unpacked, 8); //b(R0 G0 B0 A0 R0 G0 B0 A0)
			src_unpacked = _mm_mullo_pi16(src_unpacked, alpha);
			__m64 result_first = _mm_add_pi16(blend_unpacked, src_unpacked); //result = RR GG BB AA RR GG BB AA
			result_first = _mm_srli_pi16(result_first, 8);//result = 0R 0G 0B 0A 0R 0G 0B 0A
			//THIRD AND FOURTH PIXELS!
			src_unpacked = _mm_unpackhi_pi8(src_data, zero); //src_unpacked = s(0R 0G 0B 0A 0R 0G 0B 0A)
			blend_unpacked = _mm_unpackhi_pi8(blend_data, zero); //blend_unpacked = b(0R 0G 0B 0A 0R 0G 0B 0A)
			src_unpacked = _mm_sub_pi16(src_unpacked, blend_unpacked); //src_unpacked = s-b(RR GG BB AA RR GG BB AA)
			blend_unpacked = _mm_slli_pi16(blend_unpacked, 8); //b(R0 G0 B0 A0 R0 G0 B0 A0)
			src_unpacked = _mm_mullo_pi16(src_unpacked, alpha);
			__m64 result_second = _mm_add_pi16(blend_unpacked, src_unpacked); //result = RR GG BB AA RR GG BB AA
			result_second = _mm_srli_pi16(result_second, 8);//result = 0R 0G 0B 0A 0R 0G 0B 0A
			__m64 big_result = _m_packuswb(result_first, result_second);
			*reinterpret_cast<__m64*>(dst) = big_result;
			dst += 8; //4 pixels at a time.
			src += 8; //4 pixels at a time.
			blend += 8; //4 pixels at a time.
		}
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
