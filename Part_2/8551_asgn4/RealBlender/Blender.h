#pragma once

struct MyImage
{
	int* linearData;
	int height;
	int stride;
	int width;
};

EXTERN_C __declspec(dllexport) int __stdcall Blend(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor);
EXTERN_C __declspec(dllexport) int __stdcall Blend_MMX(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor);
EXTERN_C __declspec(dllexport) int __stdcall Blend_SSE(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor);