#pragma once
#include "RendererDefinitions.hpp"
#include "ShaderBufferDefinitions.hpp"
#include "ShaderDefinitions.hpp"
#include "TextureDefinitions.hpp"
#include "WindowManager/WindowManagerDefinitions.hpp"

namespace CrossRenderer
{
const char *Stringify ( const RendererBackend Value );
const char *Stringify ( const WindowEventType Value );
const char *Stringify ( const WindowManagerBackend Value );
const char *Stringify ( const PrimitiveType Value );
const char *Stringify ( const BlendMode Value );
const char *Stringify ( const StencilFunction Value );
const char *Stringify ( const StencilFailAction Value );
const char *Stringify ( const CullingMode Value );
const char *Stringify ( const CullingFaceWinding Value );
const char *Stringify ( const DepthTestMode Value );
const char *Stringify ( const ShaderUniformType Type );
const char *Stringify ( const PixelFormat Format );
}
