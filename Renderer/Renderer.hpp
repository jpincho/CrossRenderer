#pragma once
#include "RendererDefinitions.hpp"
#include "ShaderBufferDefinitions.hpp"
#include "TextureDefinitions.hpp"
#include "ShaderDefinitions.hpp"
#include "FramebufferDefinitions.hpp"
#include "RenderCommandDefinitions.hpp"
#include "Stringify.hpp"

namespace CrossRenderer
{
// Tricking intellisense into thinking these are actual functions. Very useful
#if defined (__INTELLISENSE__)
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
return name ( __VA_ARGS__ )
#else
#define DECLARE_INTERFACE_FUNCTION(return,name,...)\
extern return (*name) ( __VA_ARGS__ )
#endif
#include "RendererDeclarations.hpp"

#undef DECLARE_INTERFACE_FUNCTION
void CreateNewRenderer ( const RendererBackend &Backend );
bool SanityCheckRenderCommand ( const RenderCommand &Command );
}
