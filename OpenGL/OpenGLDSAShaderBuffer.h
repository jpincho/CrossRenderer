#pragma once
#include "../ShaderBufferDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderBufferHandle DSACreateShaderBuffer ( const ShaderBufferDescriptor CreationParameters );
bool DSAChangeShaderBufferContents ( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters );
void *DSAMapShaderBuffer ( const ShaderBufferHandle Handle, const ShaderBufferAccessType AccessType );
bool DSAUnmapShaderBuffer ( const ShaderBufferHandle Handle );
}
}
