#pragma once
#include "../GLMHeaders.hpp"
#include "../RendererHandleTemplate.hpp"
#include "../PixelFormat.hpp"
#include <vector>

namespace CrossRenderer
{
enum class TextureFilter : uint8_t
	{
	Linear = 0,
	Nearest,
	NearestMipmapNearest,
	NearestMipmapLinear,
	LinearMipmapLinear,
	LinearMipmapNearest
	};

enum class TextureWrapMode : uint8_t
	{
	Repeat = 0,
	MirroredRepeat,
	ClampToEdge,
	ClampToBorder
	};

enum class TextureType : uint8_t
	{
	Texture2D,
	TextureCubeMap
	};

struct TextureFilterSettings
	{
	TextureFilterSettings ( void )
		{
		MinFilter = MagFilter = TextureFilter::Linear;
		}
	TextureFilterSettings ( const TextureFilter NewFilter )
		{
		MinFilter = MagFilter = NewFilter;
		}
	TextureFilterSettings ( const TextureFilter NewMinFilter, const TextureFilter NewMagFilter )
		{
		MinFilter = NewMinFilter;
		MagFilter = NewMagFilter;
		}
	bool operator == ( const TextureFilterSettings &Other ) const
		{
		return ( ( MinFilter == Other.MinFilter ) && ( MagFilter == Other.MagFilter ) );
		}
	bool operator != ( const TextureFilterSettings &Other ) const
		{
		return !( operator ==( Other ) );
		}

	TextureFilter MinFilter, MagFilter;
	};

struct TextureWrapSettings
	{
	TextureWrapSettings ( void )
		{
		Horizontal = Vertical = TextureWrapMode::Repeat;
		}
	TextureWrapSettings ( const TextureWrapMode NewMode )
		{
		Horizontal = Vertical = NewMode;
		}
	TextureWrapSettings ( const TextureWrapMode NewHorizontal, const TextureWrapMode NewVertical )
		{
		Horizontal = NewHorizontal;
		Vertical = NewVertical;
		}
	bool operator == ( const TextureWrapSettings &Other ) const
		{
		return ( ( Horizontal == Other.Horizontal ) && ( Vertical == Other.Vertical ) );
		}
	bool operator != ( const TextureWrapSettings &Other ) const
		{
		return !( operator ==( Other ) );
		}
	TextureWrapMode Horizontal, Vertical;
	};

struct TextureDescriptor
	{
	TextureDescriptor ( void )
		{
		Type = TextureType::Texture2D;
		Mipmapped = true;
		}
	TextureDescriptor ( glm::uvec2 NewDimensions, PixelFormat NewTextureFormat )
		{
		Format = NewTextureFormat;
		Dimensions = NewDimensions;
		Type = TextureType::Texture2D;
		Mipmapped = true;
		}
	PixelFormat Format;
	TextureType Type;
	glm::uvec2 Dimensions;
	TextureFilterSettings FilterSettings;
	TextureWrapSettings WrapSettings;
	bool Mipmapped;
	std::vector <uint8_t> Data;
	};

struct TextureBindSettings
	{
	TextureBindSettings ( void ) = default;
	TextureBindSettings ( const TextureHandle NewHandle ) : Handle ( NewHandle )
		{
		}
	TextureBindSettings ( const TextureHandle NewHandle, const TextureWrapSettings &NewWrapSettings, const TextureFilterSettings &NewFilterSettings ) : Handle ( NewHandle ), WrapSettings ( NewWrapSettings ), FilterSettings ( NewFilterSettings )
		{
		}
	bool operator == ( const TextureBindSettings &Other ) const
		{
		return ( ( Handle == Other.Handle ) && ( WrapSettings == Other.WrapSettings ) && ( FilterSettings == Other.FilterSettings ) );
		}
	bool operator != ( const TextureBindSettings &Other ) const
		{
		return !( operator ==( Other ) );
		}
	TextureHandle Handle;
	TextureWrapSettings WrapSettings;
	TextureFilterSettings FilterSettings;
	};
}
