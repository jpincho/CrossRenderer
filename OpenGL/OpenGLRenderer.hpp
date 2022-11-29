#pragma once
#include "../RenderCommandDefinitions.hpp"
#include "../HandleTemplate.hpp"
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
bool StartFrame ( const RenderWindowHandle &Handle );
bool EndFrame ( const RenderWindowHandle &Handle );

bool RunCommand ( const RenderCommand &Command );
}
}
