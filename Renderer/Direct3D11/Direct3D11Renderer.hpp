#pragma once
#include "../RenderCommandDefinitions.hpp"
#include "../RendererHandleTemplate.hpp"
#include "Direct3D11Framebuffer.hpp"

namespace CrossRenderer
{
namespace Direct3D11
{
bool InitializeRenderer ( const RendererConfiguration &NewConfiguration );
bool ShutdownRenderer ( void );

bool StartRenderToFramebuffer ( const FramebufferHandle &Handle );
bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );
bool StartRenderToWindow ( const RenderWindowHandle &WindowHandle );
bool DisplayWindow ( const RenderWindowHandle &WindowHandle );

bool RunCommand ( const RenderCommand &Command );
}
}
