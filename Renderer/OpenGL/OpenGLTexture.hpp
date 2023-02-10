#pragma once
#include "../TextureDefinitions.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
TextureHandle CreateTexture ( const TextureDescriptor CreationParameters );
bool DeleteTexture ( const TextureHandle Handle );
bool Load2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data );
bool Update2DTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data );
bool LoadCubeMapTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] );
glm::uvec2 GetTextureDimensions ( const TextureHandle Handle );
PixelFormat GetTextureFormat ( const TextureHandle Handle );
TextureDescriptor GetTextureDescriptor ( const TextureHandle Handle );
}
}
