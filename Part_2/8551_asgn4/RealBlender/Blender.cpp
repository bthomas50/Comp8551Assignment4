﻿// Blender.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "Blender.h"

EXTERN_C __declspec(dllexport) void __stdcall Blend(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor) {
	float inv_blend_factor = 1.0f - blend_factor;
	for (int i = 0; i < blendTo->height; i++) {
		unsigned char* original_scanline_start = (unsigned char*)original->linearData + (original->stride * i);
		unsigned char* blendTo_scanline_start = (unsigned char*)blendTo->linearData + (blendTo->stride * i);
		unsigned char* finalImage_scanline_start = (unsigned char*)finalImage->linearData + (finalImage->stride * i);
		for (int j = 0; j < blendTo->width * 4; j += 4) {
			unsigned char* original_pixel = original_scanline_start + j;
			unsigned char* blend_pixel = blendTo_scanline_start + j;
			unsigned char* final_pixel = finalImage_scanline_start + j;
			for (int ch = 0; ch < 4; ch++) {
				final_pixel[ch] = (unsigned char)(original_pixel[ch] * blend_factor + blend_pixel[ch] * inv_blend_factor);
			}
		}
	}
}