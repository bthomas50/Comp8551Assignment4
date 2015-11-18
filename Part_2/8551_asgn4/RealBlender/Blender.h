#pragma once

struct MyImage
{
	int* linearData;
	int height;
	int stride;
	int width;
};

EXTERN_C __declspec(dllexport) void __stdcall Blend(MyImage* original, MyImage* blendTo, MyImage* finalImage, float blend_factor);