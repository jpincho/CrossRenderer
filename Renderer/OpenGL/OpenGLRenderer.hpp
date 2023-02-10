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

bool StartRenderToFramebuffer ( const FramebufferHandle &Handle );
bool DisplayFramebuffer ( const FramebufferHandle &Handle, const RenderWindowHandle &WindowHandle );
bool StartFrame ( const RenderWindowHandle &WindowHandle );
bool EndFrame ( const RenderWindowHandle &WindowHandle );

bool RunCommand ( const RenderCommand &Command );
}
}
