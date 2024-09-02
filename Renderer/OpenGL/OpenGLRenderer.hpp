#pragma once
#include "../RenderCommandDefinitions.hpp"
#include "../RendererHandleTemplate.hpp"
#include "OpenGLFramebuffer.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLShaderBuffer.hpp"
#include "OpenGLTexture.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
bool InitializeRenderer ( const RendererConfiguration &NewConfiguration );
bool ShutdownRenderer ( void );

bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );
bool StartRenderToWindow ( const RenderWindowHandle &WindowHandle );
bool DisplayWindow ( const RenderWindowHandle &WindowHandle );

bool RunCommand ( const RenderCommand &Command );

bool IsExtensionAvailable ( const char *Extension );
bool EnableDirectStateAccess ( const bool NewState );
bool IsDirectStateAccessEnabled ( void );
}
}
