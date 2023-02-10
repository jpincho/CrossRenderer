#pragma once
#include "../ShaderBufferDefinitions.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
ShaderBufferHandle CreateShaderBuffer ( const ShaderBufferDescriptor CreationParameters );
bool DeleteShaderBuffer ( const ShaderBufferHandle Handle );
bool ChangeShaderBufferContents ( const ShaderBufferHandle Handle, const size_t Offset, const void *Data, const size_t DataSize );
void *MapShaderBuffer ( const ShaderBufferHandle Handle, const ShaderBufferMapAccessType AccessType );
bool UnmapShaderBuffer ( const ShaderBufferHandle Handle );
const ShaderBufferDescriptor GetShaderBufferDescriptor ( const ShaderBufferHandle Handle );
}
}
