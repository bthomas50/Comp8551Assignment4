// Blender.cpp : Defines the exported functions for the DLL application.
//

#include <chrono>
#include "pch.h"
#include "Blender.h"

EXTERN_C __declspec(dllexport) int __stdcall Blend(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor) {
	float inv_blend_factor = 1.0f - blend_factor;
	std::chrono::microseconds microseconds;
	std::chrono::high_resolution_clock::time_point before = std::chrono::high_resolution_clock::now();
	

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
	std::chrono::high_resolution_clock::time_point after = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> temp = std::chrono::duration_cast<std::chrono::duration<double>>(after - before);
	microseconds = std::chrono::duration_cast<std::chrono::microseconds> (after - before);
	return (int)(after.time_since_epoch().count() - before.time_since_epoch().count());
}