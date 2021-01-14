#pragma once
#include "GLMHeaders.h"
#include "HandleTemplate.h"
#include "PixelFormat.h"

namespace CrossRenderer
{
struct TextureTag {};
typedef HandleTemplate <TextureTag> TextureHandle;
enum class TextureFilter : uint8_t
    {
    Linear = 0,
    Nearest,
    NearestMipmapNearest,
    NearestMipmapLinear,
    LinearMipmapLinear
    };

enum class TextureWrapMode : uint8_t
    {
    Repeat = 0,
    MirroredRepeat,
    ClampToEdge
    };

enum class TextureType : uint8_t
    {
    Texture2D,
    TextureCubeMap
    };

typedef struct TextureFilterSettings
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
    TextureFilter MinFilter, MagFilter;
    } TextureFilterSettings;

typedef struct TextureWrapSettings
    {
    TextureWrapSettings ( void )
        {
        Horizontal = Vertical = TextureWrapMode::ClampToEdge;
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
    TextureWrapMode Horizontal, Vertical;
    } TextureWrapSettings;

typedef struct TextureDescriptor
    {
    TextureDescriptor ( void ) = default;
    TextureDescriptor ( glm::uvec2 NewDimensions, PixelFormat NewTextureFormat )
        {
        Format = NewTextureFormat;
        Dimensions = NewDimensions;
        Pitch = NewDimensions.x;
        Type = TextureType::Texture2D;
        }
    PixelFormat Format;
    TextureType Type;
    glm::uvec2 Dimensions;
    TextureFilterSettings FilterSettings;
    TextureWrapSettings WrapSettings;
    size_t Pitch;
    } TextureDescriptor;

typedef struct TextureBindSettings
    {
    TextureBindSettings ( void ) = default;
    TextureBindSettings ( const TextureHandle NewHandle ) : Handle ( NewHandle )
        {}
    TextureBindSettings ( const TextureHandle NewHandle, const TextureWrapSettings &NewWrapSettings, const TextureFilterSettings &NewFilterSettings ) : Handle ( NewHandle ), WrapSettings ( NewWrapSettings ), FilterSettings ( NewFilterSettings )
        {}
    TextureHandle Handle;
    TextureWrapSettings WrapSettings;
    TextureFilterSettings FilterSettings;
    } TextureBindSettings;
}
