#pragma once
#include "../RenderCommandDefinitions.h"
#include "../HandleTemplate.h"
#include "OpenGLFramebuffer.h"
#include "OpenGLShader.h"
#include "OpenGLShaderBuffer.h"
#include "OpenGLTexture.h"

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
