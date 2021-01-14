#pragma once
#include "../ShaderBufferDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderBufferHandle CreateShaderBuffer ( const ShaderBufferDescriptor CreationParameters );
bool DeleteShaderBuffer ( const ShaderBufferHandle Handle );
bool ChangeShaderBufferContents ( const ShaderBufferHandle Handle, const ShaderBufferDescriptor CreationParameters );
void *MapShaderBuffer ( const ShaderBufferHandle Handle, const ShaderBufferAccessType AccessType );
bool UnmapShaderBuffer ( const ShaderBufferHandle Handle );
}
}
