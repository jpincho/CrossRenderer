#pragma once
#include <stdint.h>

namespace CrossRenderer
{
enum class PixelFormat : uint8_t
    {
    RedGreenBlue888,
    RedGreenBlueAlpha8888,
    DepthComponent,
    };
}
