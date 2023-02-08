#include "OpenGLTexture.hpp"
#include "OpenGLInternals.hpp"
#include "../Logger.hpp"

namespace CrossRenderer
{
namespace OpenGL
{
// Texture creation
// OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_RED_INTEGER, GL_RG_INTEGER, GL_RGB_INTEGER, GL_BGR_INTEGER, GL_RGBA_INTEGER, GL_BGRA_INTEGER, GL_STENCIL_INDEX, GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL.
// DataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.
// DestinationInternalFormat - GL_DEPTH_COMPONENT, GL_DEPTH_STENCIL, GL_RED, GL_RG, GL_RGB, GL_RGBA
// (Sized) - GL_R8, GL_R8_SNORM, GL_R16, GL_R16_SNORM, GL_RG8, GL_RG8_SNORM, GL_RG16, GL_RG16_SNORM, GL_R3_G3_B2, GL_RGB4, GL_RGB5, GL_RGB8, GL_RGB8_SNORM, GL_RGB10, GL_RGB12, GL_RGB16_SNORM, GL_RGBA2, GL_RGBA4, GL_RGB5_A1
// GL_RGBA8, GL_RGBA8_SNORM, GL_RGB10_A2, GL_RGB10_A2UI, GL_RGBA12, GL_RGBA16, GL_SRGB8, GL_SRGB8_ALPHA8, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F, GL_R11F_G11F_B10F, GL_RGB9_E5
// GL_R8I, GL_R8UI, GL_R16I, GL_R16UI, GL_R32I, GL_R32UI, GL_RG8I, GL_RG8UI, GL_RG16I, GL_RG16UI, GL_RG32I, GL_RG32UI, GL_RGB8I, GL_RGB8UI, GL_RGB16I, GL_RGB16UI, GL_RGB32I, GL_RGB32UI, GL_RGBA8I, GL_RGBA8UI, GL_RGBA16I, GL_RGBA16UI, GL_RGBA32I, GL_RGBA32UI
// (Compressed) - GL_COMPRESSED_RED, GL_COMPRESSED_RG, GL_COMPRESSED_RGB, GL_COMPRESSED_RGBA, GL_COMPRESSED_SRGB, GL_COMPRESSED_SRGB_ALPHA, GL_COMPRESSED_RED_RGTC1, GL_COMPRESSED_SIGNED_RED_RGTC1, GL_COMPRESSED_RG_RGTC2
// GL_COMPRESSED_SIGNED_RG_RGTC2, GL_COMPRESSED_RGBA_BPTC_UNORM, GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM, GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT, GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT

// Texture update
// OriginPixelFormat - GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
// DataType - GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, GL_UNSIGNED_BYTE_3_3_2, GL_UNSIGNED_BYTE_2_3_3_REV, GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_5_6_5_REV, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_4_4_4_4_REV, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_SHORT_1_5_5_5_REV, GL_UNSIGNED_INT_8_8_8_8, GL_UNSIGNED_INT_8_8_8_8_REV, GL_UNSIGNED_INT_10_10_10_2, and GL_UNSIGNED_INT_2_10_10_10_REV.

static struct
    {
    GLint DataType;
    GLint Format;
    GLint SizedFormat;
    } PixelFormatData[] =
    {
		{GL_UNSIGNED_BYTE, GL_RED, GL_R8},// Red8
		{GL_UNSIGNED_BYTE, GL_RG, GL_RG8},// RedGreen88
        {GL_UNSIGNED_BYTE, GL_RGB, GL_RGB8},// RedGreenBlue888
        {GL_UNSIGNED_BYTE, GL_RGBA, GL_RGBA8},// RedGreenBlueAlpha8888
        {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24}// DepthComponent
    };

TextureHandle CreateTexture ( const TextureDescriptor CreationParameters )
    {
    TextureInfo NewTexture;
    NewTexture = CreationParameters;
    NewTexture.GLSWrap = Translate ( CreationParameters.WrapSettings.Horizontal );
    NewTexture.GLTWrap = Translate ( CreationParameters.WrapSettings.Vertical );
    NewTexture.GLMinFilter = Translate ( CreationParameters.FilterSettings.MinFilter );
    NewTexture.GLMagFilter = Translate ( CreationParameters.FilterSettings.MagFilter );

    switch ( NewTexture.Type )
        {
        case TextureType::Texture2D:
            {
            glGenTextures ( 1, &NewTexture.OpenGLID );
            if ( CheckError () == false )
                {
				LOG_ERROR ( "Unable to create 2D texture" );
                return TextureHandle::Invalid;
                }
            glBindTexture ( GL_TEXTURE_2D, NewTexture.OpenGLID );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, NewTexture.GLSWrap );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, NewTexture.GLTWrap );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, NewTexture.GLMinFilter );
            glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, NewTexture.GLMagFilter );
            glTexStorage2D ( GL_TEXTURE_2D,
                             1,
                             PixelFormatData[ ( int ) NewTexture.Format].SizedFormat,
                             NewTexture.Dimensions.x, NewTexture.Dimensions.y );
            break;
            }
        case TextureType::TextureCubeMap:
            {
            glGenTextures ( 1, &NewTexture.OpenGLID );
            if ( CheckError () == false )
                {
				LOG_ERROR ( "Unable to create cubemap texture" );
                return TextureHandle::Invalid;
                }
            glBindTexture ( GL_TEXTURE_CUBE_MAP, NewTexture.OpenGLID );
            glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, NewTexture.GLSWrap );
            glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, NewTexture.GLTWrap );
            glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, NewTexture.GLMinFilter );
            glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, NewTexture.GLMagFilter );
            glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
            glTexStorage2D ( GL_TEXTURE_CUBE_MAP,
                             1,
                             PixelFormatData[ ( int ) NewTexture.Format].SizedFormat,
                             NewTexture.Dimensions.x, NewTexture.Dimensions.y );
            break;
            }
        default:
			LOG_ERROR ( "Unhandled texture type for creation" );
            return TextureHandle::Invalid;
        }

    if ( CheckError () == false )
        {
        glDeleteTextures ( 1, &NewTexture.OpenGLID );
        return TextureHandle::Invalid;
        }

    TextureHandle NewHandle ( Textures.GetFreeIndex () );
    Textures[NewHandle] = NewTexture;
    return NewHandle;
    }

bool DeleteTexture ( const TextureHandle Handle )
    {
    TextureInfo *TextureInformation = &Textures[Handle];

    glDeleteTextures ( 1, &TextureInformation->OpenGLID );
    Textures.ReleaseIndex ( Handle );
    return true;
    }

bool Load2DTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, const void *Data )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    return Update2DTextureRegion ( Handle, glm::uvec2 ( 0, 0 ), TextureInformation->Dimensions, SourcePixelFormat, Data );
    }

bool Update2DTextureRegion ( const TextureHandle Handle, const glm::uvec2 LowerLeft, const glm::uvec2 RegionDimensions, const PixelFormat SourcePixelFormat, const void *Data )
    {
    TextureInfo *TextureInformation = &Textures[Handle];

    if ( ( LowerLeft.x + RegionDimensions.x > TextureInformation->Dimensions.x ) ||
            ( LowerLeft.y + RegionDimensions.y > TextureInformation->Dimensions.y ) )
        return false;

    uint8_t alignment = 1;
    uint32_t alignment_mask;
    for ( alignment_mask = 1; alignment_mask <= 8; alignment_mask *= 2 )
        {
        if ( ( RegionDimensions.x % alignment_mask == 0 ) && ( reinterpret_cast<long long> ( Data ) % alignment_mask == 0 ) )
            alignment = alignment_mask & 0xFF;
        }
    glPixelStorei ( GL_UNPACK_ALIGNMENT, alignment );
    glBindTexture ( GL_TEXTURE_2D, TextureInformation->OpenGLID );
    glTexSubImage2D ( GL_TEXTURE_2D,
                      0,
                      LowerLeft.x, LowerLeft.y,
                      ( int ) RegionDimensions.x, ( int ) RegionDimensions.y,
                      PixelFormatData[ ( int ) SourcePixelFormat].Format,
                      PixelFormatData[ ( int ) SourcePixelFormat].DataType,
                      Data );
    if ( !CheckError () )
        return false;
    if ( TextureInformation->Mipmapped )
        glGenerateMipmap ( GL_TEXTURE_2D );

    return true;
    }

bool LoadCubeMapTextureData ( const TextureHandle Handle, const PixelFormat SourcePixelFormat, void *Data[6] )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    glBindTexture ( GL_TEXTURE_CUBE_MAP, TextureInformation->OpenGLID );
    for ( unsigned Face = 0; Face < 6; ++Face )
        {
        glTexSubImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face,
                          0,
                          0, 0,
                          TextureInformation->Dimensions.x, TextureInformation->Dimensions.y,
                          PixelFormatData[ ( int ) SourcePixelFormat].Format,
                          PixelFormatData[ ( int ) SourcePixelFormat].DataType,
                          Data[Face] );
        }
    if ( TextureInformation->Mipmapped )
        glGenerateMipmap ( GL_TEXTURE_CUBE_MAP );

    glBindTexture ( GL_TEXTURE_CUBE_MAP, 0 );
    return CheckError ();
    }

glm::uvec2 GetTextureDimensions ( const TextureHandle Handle )
    {
    TextureInfo *TextureInformation = &Textures[Handle];
    return TextureInformation->Dimensions;
    }

PixelFormat GetTextureFormat ( const TextureHandle Handle )
	{
	TextureInfo *TextureInformation = &Textures[Handle];
	return TextureInformation->Format;
	}

TextureDescriptor GetTextureDescriptor ( const TextureHandle Handle )
	{
	return Textures[Handle];
	}
}
}
