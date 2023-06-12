#pragma once
#include "../WindowManager/WindowManagerDefinitions.hpp"
#include <vector>

namespace CrossRenderer
{
enum class RendererBackend
    {
    OpenGLCore,
	Direct3D11
    };

struct RendererConfiguration
    {
    RendererConfiguration ( void )
        {
        VSyncEnabled = true;
        RedBits = GreenBits = BlueBits = AlphaBits = 8;
        DepthBits = 24;
        StencilBits = 8;
        DesiredRendererBackend = ( RendererBackend ) 0;
        DesiredWindowBackend = (WindowManager::WindowManagerBackend ) 0;
        }
    std::vector <WindowManager::RenderWindowDescriptor> InitialWindowConfigurations;
	WindowManager::WindowManagerBackend DesiredWindowBackend;
    RendererBackend DesiredRendererBackend;
    bool VSyncEnabled;
    uint8_t RedBits, GreenBits, BlueBits, AlphaBits, DepthBits, StencilBits;
    };
}
