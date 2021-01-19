#pragma once
#include "../TextureDefinitions.h"

namespace CrossRenderer
{
namespace OpenGL
{
TextureHandle DSACreateTexture ( const TextureDescriptor CreationParameters );
bool DSALoad2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data );
bool DSAUpdate2DTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data );
bool DSALoadCubeMapTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] );
}
}
