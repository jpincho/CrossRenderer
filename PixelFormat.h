#pragma once
#include <stdint.h>

namespace CrossRenderer
{
enum class PixelFormat : uint8_t
    {
    Alpha = 0,
    RedGreenBlue888,
    RedGreenBlue565,
    RedGreenBlueAlpha8888,
    BlueGreenRed888,
    BlueGreenRedAlpha8888,

    AlphaRedGreenBlue8888,
    AlphaBlueGreenRed8888,

    // Texture format only
    DepthComponent,
    };
}
