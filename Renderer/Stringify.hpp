#pragma once
#include "RendererDefinitions.hpp"
#include "RenderCommandDefinitions.hpp"
#include "ShaderBufferDefinitions.hpp"
#include "ShaderDefinitions.hpp"
#include "TextureDefinitions.hpp"

namespace CrossRenderer
{
const char *Stringify ( const RendererBackend Value );
const char *Stringify ( const PrimitiveType Value );
const char *Stringify ( const BlendMode Value );
const char *Stringify ( const StencilFunction Value );
const char *Stringify ( const StencilFailAction Value );
const char *Stringify ( const CullingMode Value );
const char *Stringify ( const CullingFaceWinding Value );
const char *Stringify ( const DepthTestMode Value );
const char *Stringify ( const ShaderUniformType Value );
const char *Stringify ( const PixelFormat Value );
const char *Stringify ( const ShaderObjectType Value );
const char *Stringify ( const TextureWrapMode Value );
const char *Stringify ( const TextureFilter Value );
const char *Stringify ( const PolygonMode Value );
}
