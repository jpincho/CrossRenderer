#pragma once
#include "../TextureDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
TextureHandle Create2DTexture ( const TextureDescriptor CreationParameters );
TextureHandle DSACreate2DTexture ( const TextureDescriptor CreationParameters );
bool DeleteTexture ( const TextureHandle Handle );
bool Load2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data, const unsigned Pitch );
bool DSALoad2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data, const unsigned Pitch );
bool UpdateTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const void *Data );
bool DSAUpdateTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const void *Data );
glm::uvec2 GetTextureDimensions ( const TextureHandle Handle );
}
}
