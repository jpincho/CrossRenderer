#pragma once
#include <stdint.h>

namespace CrossRenderer
{
enum class PixelFormat : uint8_t
	{
	Red8,
	RedGreen88,
	RedGreenBlue888,
	RedGreenBlueAlpha8888,
	AlphaRedGreenBlue8888,
	RedGreenBlue565,
	DepthComponent,
	};
}
